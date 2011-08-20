/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include <zip.h>

#include "orcus/global.hpp"
#include "orcus/xml_parser.hpp"
#include "orcus/xml_simple_handler.hpp"
#include "orcus/ooxml/global.hpp"
#include "orcus/ooxml/xlsx_handler.hpp"
#include "orcus/ooxml/xlsx_context.hpp"
#include "orcus/ooxml/opc_context.hpp"
#include "orcus/ooxml/ooxml_tokens.hpp"
#include "orcus/ooxml/schemas.hpp"
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/sheet.hpp"
#include "orcus/model/factory.hpp"
#include "orcus/model/document.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;
using namespace orcus;

namespace {

class print_xml_content_types : unary_function<void, xml_part_t>
{
public:
    print_xml_content_types(const char* prefix) :
        m_prefix(prefix) {}

    void operator() (const xml_part_t& v) const
    {
        cout << "* " << m_prefix << ": " << v.first;
        if (v.second)
            cout << " (" << v.second << ")";
        else
            cout << " (<unknown content type>)";
        cout << endl;
    }
private:
    const char* m_prefix;
};

struct print_sheet_info : unary_function<void, pair<pstring, const opc_rel_extra*> >
{
    void operator() (const pair<pstring, const opc_rel_extra*>& v) const
    {
        const xlsx_rel_sheet_info* info = static_cast<const xlsx_rel_sheet_info*>(v.second);
        cout << "sheet name: " << info->name << "  sheet id: " << info->id << "  relationship id: " << v.first << endl;
    }
};

class orcus_xlsx : public ::boost::noncopyable
{
public:
    orcus_xlsx(model::factory_base* factory);
    ~orcus_xlsx();

    void read_file(const char* fpath);

    /**
     * Read an xml part inside package.  The path is relative to the relation
     * file.
     *
     * @param path the path to the xml part.
     * @param type schema type.
     */
    void read_part(const pstring& path, const schema_t type, const opc_rel_extra* data);

private:
    void list_content() const;
    void read_content();
    void read_content_types();
    void read_relations(const char* path, vector<opc_rel_t>& rels);
    void read_workbook(const char* file_name);

    /**
     * Parse a sheet xml part that contains data stored in a single sheet.
     */
    void read_sheet(const char* file_name, const xlsx_rel_sheet_info* data);

    /**
     * Parse sharedStrings.xml part that contains a list of strings referenced
     * in the document.
     */
    void read_shared_strings(const char* file_name);

    void read_styles(const char* file_name);

    void check_relation_part(const char* file_name, const opc_rel_extras_t* extras);

    string get_current_dir() const;

private:
    typedef vector<string> dir_stack_type;

    model::factory_base* mp_factory;
    struct zip* m_archive;

    xml_simple_stream_handler m_opc_rel_handler;

    vector<xml_part_t> m_parts;
    vector<xml_part_t> m_ext_defaults;
    dir_stack_type m_dir_stack;
};

struct process_opc_rel : public unary_function<void, opc_rel_t>
{
    process_opc_rel(orcus_xlsx& parent, const opc_rel_extras_t* extras) :
        m_parent(parent), m_extras(extras) {}

    void operator() (const opc_rel_t& v)
    {
        const opc_rel_extra* data = NULL;
        if (m_extras)
        {
            opc_rel_extras_t::const_iterator itr = m_extras->find(v.rid);
            if (itr != m_extras->end())
                data = itr->second;
        }
        m_parent.read_part(v.target, v.type, data);
    }
private:
    orcus_xlsx& m_parent;
    const opc_rel_extras_t* m_extras;
};

struct zip_file* get_zip_stream_from_archive(
    struct zip* archive, const string& filepath, vector<uint8_t>& buf, int& buf_read)
{
    buf_read = 0;
    struct zip_stat file_stat;
    if (zip_stat(archive, filepath.c_str(), 0, &file_stat))
    {
        cout << "failed to get stat on " << filepath << endl;
        return NULL;
    }

    cout << "name: " << file_stat.name << "  size: " << file_stat.size << endl;
    struct zip_file* zfd = zip_fopen(archive, file_stat.name, 0);
    if (!zfd)
    {
        cout << "failed to open " << file_stat.name << endl;
        return NULL;
    }

    vector<uint8_t> _buf(file_stat.size, 0);
    buf_read = zip_fread(zfd, &_buf[0], file_stat.size);
    cout << "actual buffer read: " << buf_read << endl;

    buf.swap(_buf);
    return zfd;
}

orcus_xlsx::orcus_xlsx(model::factory_base* factory) :
    mp_factory(factory),
    m_archive(NULL),
    m_opc_rel_handler(new opc_relations_context(opc_tokens)) {}

orcus_xlsx::~orcus_xlsx() {}

void orcus_xlsx::read_file(const char* fpath)
{
    cout << "reading " << fpath << endl;

    int error;
    m_archive = zip_open(fpath, 0, &error);
    if (!m_archive)
    {
        cout << "failed to open " << fpath << endl;
        return;
    }

    m_dir_stack.push_back(string()); // push root directory.

    list_content();
    read_content();

    zip_close(m_archive);
}

void orcus_xlsx::read_part(const pstring& path, schema_t type, const opc_rel_extra* data)
{
    assert(!m_dir_stack.empty());

    dir_stack_type dir_changed;

    // Change current directory and read the in-file.
    const char* p = path.get();
    const char* p_name = NULL;
    size_t name_len = 0;
    for (size_t i = 0, n = path.size(); i < n; ++i, ++p)
    {
        if (!p_name)
            p_name = p;

        ++name_len;

        if (*p == '/')
        {
            // Push a new directory.
            string dir_name(p_name, name_len);
            if (dir_name == "..")
            {
                dir_changed.push_back(m_dir_stack.back());
                m_dir_stack.pop_back();
            }
            else
            {
                m_dir_stack.push_back(dir_name);

                // Add a null directory to the change record to remove it at the end.
                dir_changed.push_back(string());
            }

            p_name = NULL;
            name_len = 0;
        }
    }

    if (p_name)
    {
        // This is a file.
        string file_name(p_name, name_len);

        if (type == SCH_od_rels_office_doc)
            read_workbook(file_name.c_str());
        else if (type == SCH_od_rels_worksheet)
            read_sheet(file_name.c_str(), static_cast<const xlsx_rel_sheet_info*>(data));
        else if (type == SCH_od_rels_shared_strings)
            read_shared_strings(file_name.c_str());
        else if (type == SCH_od_rels_styles)
            read_styles(file_name.c_str());
        else
        {
            cout << "---" << endl;
            cout << "unhandled relationship type: " << type << endl;
        }
    }

    // Unwind to the original directory.
    while (!dir_changed.empty())
    {
        const string& dir = dir_changed.back();
        if (dir.empty())
            // remove added directory.
            m_dir_stack.pop_back();
        else
            // re-add removed directory.
            m_dir_stack.push_back(dir);

        dir_changed.pop_back();
    }
}

void orcus_xlsx::list_content() const
{
    zip_uint64_t num = zip_get_num_entries(m_archive, 0);
    cout << "number of files this archive contains: " << num << endl;

    for (zip_uint64_t i = 0; i < num; ++i)
    {
        const char* filename = zip_get_name(m_archive, i, 0);
        cout << filename << endl;
    }
}

void orcus_xlsx::read_content()
{
    if (m_dir_stack.empty())
        return;

    // [Content_Types].xml

    read_content_types();
    for_each(m_parts.begin(), m_parts.end(), print_xml_content_types("part name"));
    for_each(m_ext_defaults.begin(), m_ext_defaults.end(), print_xml_content_types("extension default"));

    // _rels/.rels

    m_dir_stack.push_back(string("_rels/"));
    vector<opc_rel_t> rels;
    read_relations(".rels", rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, NULL));
}

void orcus_xlsx::read_content_types()
{
    string filepath("[Content_Types].xml");
    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    if (buf_read > 0)
    {
        xml_stream_parser parser(opc_tokens, &buf[0], buf_read, "[Content_Types].xml");
        ::boost::scoped_ptr<xml_simple_stream_handler> handler(
            new xml_simple_stream_handler(new opc_content_types_context(opc_tokens)));
        parser.set_handler(handler.get());
        parser.parse();

        opc_content_types_context& context =
            static_cast<opc_content_types_context&>(handler->get_context());
        context.pop_parts(m_parts);
        context.pop_ext_defaults(m_ext_defaults);
    }
    zip_fclose(zfd);
}

void orcus_xlsx::read_relations(const char* path, vector<opc_rel_t>& rels)
{
    string filepath = get_current_dir() + path;
    cout << "file path: " << filepath << endl;

    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    if (buf_read > 0)
    {
        xml_stream_parser parser(opc_tokens, &buf[0], buf_read, filepath);

        opc_relations_context& context =
            static_cast<opc_relations_context&>(m_opc_rel_handler.get_context());
        context.init();
        parser.set_handler(&m_opc_rel_handler);
        parser.parse();
        context.pop_rels(rels);
    }
    zip_fclose(zfd);
}

void orcus_xlsx::read_workbook(const char* file_name)
{
    string filepath = get_current_dir() + file_name;
    cout << "read_workbook: file path = " << filepath << endl;

    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_workbook_context(ooxml_tokens)));

    if (buf_read > 0)
    {
        xml_stream_parser parser(ooxml_tokens, &buf[0], buf_read, filepath);
        parser.set_handler(handler.get());
        parser.parse();
    }
    zip_fclose(zfd);

    // Get sheet info from the context instance.
    xlsx_workbook_context& context =
        static_cast<xlsx_workbook_context&>(handler->get_context());
    opc_rel_extras_t sheet_data;
    context.pop_sheet_info(sheet_data);
    for_each(sheet_data.begin(), sheet_data.end(), print_sheet_info());

    check_relation_part(file_name, &sheet_data);
}

void orcus_xlsx::read_sheet(const char* file_name, const xlsx_rel_sheet_info* data)
{
    cout << "---" << endl;
    string filepath = get_current_dir() + file_name;
    cout << "read_sheet: file path = " << filepath << endl;

    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    if (data)
    {
        cout << "relationship sheet data: " << endl;
        cout << "  sheet name: " << data->name << "  sheet ID: " << data->id << endl;
    }

    if (buf_read > 0)
    {
        xml_stream_parser parser(ooxml_tokens, &buf[0], buf_read, file_name);
        model::sheet_base* sheet = mp_factory->append_sheet(data->name.get(), data->name.size());
        ::boost::scoped_ptr<xlsx_sheet_xml_handler> handler(new xlsx_sheet_xml_handler(ooxml_tokens, sheet));
        parser.set_handler(handler.get());
        parser.parse();
    }
    zip_fclose(zfd);

    check_relation_part(file_name, NULL);
}

void orcus_xlsx::read_shared_strings(const char* file_name)
{
    cout << "---" << endl;
    string filepath = get_current_dir() + file_name;
    cout << "read_shared_strings: file path = " << filepath << endl;

    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    if (buf_read > 0)
    {
        xml_stream_parser parser(ooxml_tokens, &buf[0], buf_read, file_name);
        ::boost::scoped_ptr<xml_simple_stream_handler> handler(
            new xml_simple_stream_handler(new xlsx_shared_strings_context(ooxml_tokens, mp_factory->get_shared_strings())));
        parser.set_handler(handler.get());
        parser.parse();
    }
    zip_fclose(zfd);
}

void orcus_xlsx::read_styles(const char* file_name)
{
    cout << "---" << endl;
    string filepath = get_current_dir() + file_name;
    cout << "read_styles: file path = " << filepath << endl;

    vector<uint8_t> buf;
    int buf_read;
    struct zip_file* zfd = get_zip_stream_from_archive(m_archive, filepath, buf, buf_read);
    if (!zfd)
        return;

    if (buf_read > 0)
    {
        xml_stream_parser parser(ooxml_tokens, &buf[0], buf_read, file_name);
        ::boost::scoped_ptr<xml_simple_stream_handler> handler(
            new xml_simple_stream_handler(new xlsx_styles_context(ooxml_tokens, mp_factory->get_styles())));
//      xlsx_styles_context& context =
//          static_cast<xlsx_styles_context&>(handler->get_context());
        parser.set_handler(handler.get());
        parser.parse();
    }
    zip_fclose(zfd);
}

void orcus_xlsx::check_relation_part(const char* file_name, const opc_rel_extras_t* extra)
{
    // Read the relationship file associated with this file, located at
    // _rels/<file name>.rels.
    vector<opc_rel_t> rels;
    m_dir_stack.push_back(string("_rels/"));
    string rels_file_name = string(file_name) + ".rels";
    read_relations(rels_file_name.c_str(), rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, extra));
}

string orcus_xlsx::get_current_dir() const
{
    string pwd;
    vector<string>::const_iterator itr = m_dir_stack.begin(), itr_end = m_dir_stack.end();
    for (; itr != itr_end; ++itr)
        pwd += *itr;
    return pwd;
}

}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    ::boost::scoped_ptr<model::document> doc(new model::document);
    ::boost::scoped_ptr<model::factory> factory(new model::factory(doc.get()));
    orcus_xlsx app(factory.get());
    app.read_file(argv[1]);
//  doc->dump();
//  doc->dump_html("./obj");
//  pstring::intern::dump();
    pstring::intern::dispose();
    return EXIT_SUCCESS;
}
