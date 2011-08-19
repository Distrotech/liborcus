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

#define USE_LIBZIP 1

#if USE_LIBZIP
#include <zip.h>
#else
#include <gsf/gsf-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-zip.h>
#endif

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

#if USE_LIBZIP

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
//      else if (type == SCH_od_rels_worksheet)
//          read_sheet(file_name.c_str(), static_cast<const xlsx_rel_sheet_info*>(data));
//      else if (type == SCH_od_rels_shared_strings)
//          read_shared_strings(file_name.c_str());
//      else if (type == SCH_od_rels_styles)
//          read_styles(file_name.c_str());
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
    struct zip_stat file_stat;
    if (zip_stat(m_archive, "[Content_Types].xml", 0, &file_stat))
    {
        cout << "failed to get stat on [Content_Types].xml" << endl;
        return;
    }

    cout << "name: " << file_stat.name << "  size: " << file_stat.size << endl;
    struct zip_file* zfd = zip_fopen(m_archive, file_stat.name, 0);
    if (!zfd)
    {
        cout << "failed to open " << file_stat.name << endl;
        return;
    }

    vector<uint8_t> buf(file_stat.size, 0);
    int buf_read = zip_fread(zfd, &buf[0], file_stat.size);
    cout << "actual buffer read: " << buf_read << endl;
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

    struct zip_stat file_stat;
    if (zip_stat(m_archive, filepath.c_str(), 0, &file_stat))
    {
        cout << "failed to get stat on " << filepath << endl;
        return;
    }

    cout << "name: " << file_stat.name << "  size: " << file_stat.size << endl;
    struct zip_file* zfd = zip_fopen(m_archive, file_stat.name, 0);
    if (!zfd)
    {
        cout << "failed to open " << file_stat.name << endl;
        return;
    }

    vector<uint8_t> buf(file_stat.size, 0);
    int buf_read = zip_fread(zfd, &buf[0], file_stat.size);
    cout << "actual buffer read: " << buf_read << endl;
    if (buf_read > 0)
    {
        xml_stream_parser parser(opc_tokens, &buf[0], file_stat.size, filepath);

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

    struct zip_stat file_stat;
    if (zip_stat(m_archive, filepath.c_str(), 0, &file_stat))
    {
        cout << "failed to get stat on " << filepath << endl;
        return;
    }

    cout << "name: " << file_stat.name << "  size: " << file_stat.size << endl;
    struct zip_file* zfd = zip_fopen(m_archive, file_stat.name, 0);
    if (!zfd)
    {
        cout << "failed to open " << file_stat.name << endl;
        return;
    }

    vector<uint8_t> buf(file_stat.size, 0);
    int buf_read = zip_fread(zfd, &buf[0], file_stat.size);
    cout << "actual buffer read: " << buf_read << endl;

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

#else

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

class gsf_infile_guard
{
public:
    gsf_infile_guard(GsfInput* parent, const char* name, bool throw_on_failure = true) :
        mp_input(NULL)
    {
        if (name)
            mp_input = gsf_infile_child_by_name(GSF_INFILE(parent), name);
        else
            mp_input = parent;

        if (!mp_input && throw_on_failure)
        {
            ostringstream os;
            os << "failed to open infile child: '" << name << "'";
            throw general_error(os.str());
        }
    }

    gsf_infile_guard(const gsf_infile_guard& r) :
        mp_input(r.mp_input)
    {
        if (mp_input)
            g_object_ref(G_OBJECT(mp_input));
    }

    ~gsf_infile_guard()
    {
        if (mp_input)
            g_object_unref(G_OBJECT(mp_input));
    }

    GsfInput* get() const { return mp_input; }

private:
    GsfInput* mp_input;
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
    /**
     * Parse the [Content_Types].xml part to extract the paths and content
     * types of the other xml parts contained in the package. This is the
     * first part in the package to be parsed.
     */
    void read_content_types();

    void check_relation_part(const char* file_name, const opc_rel_extras_t* extras);

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

    /**
     * The top-level function that determines the order in which the
     * individual parts get parsed.
     */
    void read_content();

    void list_content (GsfInput* input, int level = 0);

private:
    xml_simple_stream_handler m_opc_rel_handler;

    vector<xml_part_t> m_parts;
    vector<xml_part_t> m_ext_defaults;
    vector<gsf_infile_guard> m_dir_stack;

    model::factory_base* mp_factory;
};

orcus_xlsx::orcus_xlsx(model::factory_base* factory) :
    m_opc_rel_handler(new opc_relations_context(opc_tokens)),
    mp_factory(factory) {}

orcus_xlsx::~orcus_xlsx() {}

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

void orcus_xlsx::read_file(const char* fpath)
{
    cout << "reading " << fpath << endl;

    gsf_init();

    GError* err = NULL;
    GsfInput* input = gsf_input_stdio_new (fpath, &err);
    if (!input)
    {
        g_error_free (err);
        return;
    }

    GsfInfile* infile = gsf_infile_zip_new (input, &err);
    g_object_unref (G_OBJECT (input));
    if (!infile)
    {
        g_error_free (err);
        return;
    }

    gsf_infile_guard dir_root(GSF_INPUT(infile), NULL);
    m_dir_stack.push_back(dir_root);
//  list_content(GSF_INPUT(infile));
    read_content();

    gsf_shutdown();
}

void orcus_xlsx::read_part(const pstring& path, schema_t type, const opc_rel_extra* data)
{
    assert(!m_dir_stack.empty());

    // Keep track of directovy movement to unwind to the original directory at
    // the end of this method.
    vector<gsf_infile_guard> dir_changed;

    // Change current directory and read the in-file.
    const char* p = path.get();
    const char* p_name = NULL;
    size_t name_len = 0;
    for (size_t i = 0, n = path.size(); i < n; ++i, ++p)
    {
        if (!p_name)
            p_name = p;

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
                gsf_infile_guard dir_new(m_dir_stack.back().get(), dir_name.c_str());
                m_dir_stack.push_back(dir_new);

                // Add a null directory to the change record to remove it at the end.
                dir_changed.push_back(gsf_infile_guard(NULL, NULL, false));
            }

            p_name = NULL;
            name_len = 0;
        }
        else
            ++name_len;
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
        const gsf_infile_guard& dir = dir_changed.back();
        if (dir.get())
            // re-add removed directory.
            m_dir_stack.push_back(dir);
        else
            // remove added directory.
            m_dir_stack.pop_back();

        dir_changed.pop_back();
    }
}

void orcus_xlsx::read_content_types()
{
    gsf_infile_guard guard(m_dir_stack.back().get(), "[Content_Types].xml");
    GsfInput* input = guard.get();
    if (!input)
        throw general_error("could not open [Content_Types].xml part.");

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "name: [Content_Types].xml  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, "[Content_Types].xml");
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new opc_content_types_context(opc_tokens)));
    opc_content_types_context& context =
        static_cast<opc_content_types_context&>(handler->get_context());
    parser.set_handler(handler.get());
    parser.parse();
    context.pop_parts(m_parts);
    context.pop_ext_defaults(m_ext_defaults);
}

void orcus_xlsx::check_relation_part(const char* file_name, const opc_rel_extras_t* extra)
{
    // Read the relationship file associated with this file, located at
    // _rels/<file name>.rels.
    vector<opc_rel_t> rels;
    gsf_infile_guard dir_rels(m_dir_stack.back().get(), "_rels", false);
    if (!dir_rels.get())
        // _rels directory not found.  Nothing to do.
        return;

    m_dir_stack.push_back(dir_rels);
    string rels_file_name(file_name);
    rels_file_name += ".rels";
    read_relations(rels_file_name.c_str(), rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, extra));
}

void orcus_xlsx::read_relations(const char* path, vector<opc_rel_t>& rels)
{
    GsfInput* dir_cur = m_dir_stack.back().get();
    gsf_infile_guard rels_guard(dir_cur, path, false);
    GsfInput* input_rels = rels_guard.get();
    if (!input_rels)
        // Specified .rels file not found.  Bail out.
        return;

    size_t size = gsf_input_size(input_rels);
    cout << "---" << endl;
    cout << "name: " << path << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input_rels, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, path);

    opc_relations_context& context =
        static_cast<opc_relations_context&>(m_opc_rel_handler.get_context());
    context.init();
    parser.set_handler(&m_opc_rel_handler);
    parser.parse();
    context.pop_rels(rels);
}

void orcus_xlsx::read_workbook(const char* file_name)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "file name: " << file_name << "  size: " << size << endl;

    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_workbook_context(ooxml_tokens)));
    xlsx_workbook_context& context =
        static_cast<xlsx_workbook_context&>(handler->get_context());
    parser.set_handler(handler.get());
    parser.parse();

    // Get sheet info from the context instance.
    opc_rel_extras_t sheet_data;
    context.pop_sheet_info(sheet_data);
    for_each(sheet_data.begin(), sheet_data.end(), print_sheet_info());

    check_relation_part(file_name, &sheet_data);
}

void orcus_xlsx::read_sheet(const char* file_name, const xlsx_rel_sheet_info* data)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    if (data)
    {
        cout << "relationship sheet data: " << endl;
        cout << "  sheet name: " << data->name << "  sheet ID: " << data->id << endl;
    }
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    model::sheet_base* sheet = mp_factory->append_sheet(data->name.get(), data->name.size());
    ::boost::scoped_ptr<xlsx_sheet_xml_handler> handler(new xlsx_sheet_xml_handler(ooxml_tokens, sheet));
    parser.set_handler(handler.get());
    parser.parse();

    check_relation_part(file_name, NULL);
}

void orcus_xlsx::read_shared_strings(const char* file_name)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);

    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_shared_strings_context(ooxml_tokens, mp_factory->get_shared_strings())));
    parser.set_handler(handler.get());
    parser.parse();
}

void orcus_xlsx::read_styles(const char* file_name)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);

    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_styles_context(ooxml_tokens, mp_factory->get_styles())));
//  xlsx_styles_context& context =
//      static_cast<xlsx_styles_context&>(handler->get_context());
    parser.set_handler(handler.get());
    parser.parse();
}

void orcus_xlsx::read_content()
{
    if (m_dir_stack.empty())
        return;

    // Get the root directory handle.
    GsfInput* dir_root = m_dir_stack.back().get();

    // [Content_Types].xml

    read_content_types();
    for_each(m_parts.begin(), m_parts.end(), print_xml_content_types("part name"));
    for_each(m_ext_defaults.begin(), m_ext_defaults.end(), print_xml_content_types("extension default"));

    // _rels/.rels

    vector<opc_rel_t> rels;

    gsf_infile_guard dir_rels(dir_root, "_rels");
    m_dir_stack.push_back(dir_rels);
    read_relations(".rels", rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, NULL));
}

void orcus_xlsx::list_content (GsfInput* input, int level)
{
    if (!GSF_IS_INFILE(input))
        return;

    int child_count = gsf_infile_num_children (GSF_INFILE (input));
    bool is_dir = child_count >= 0;

    for (int i = 0; i < level; ++i)
        printf("   ");

    const char* name = gsf_input_name(input);
    size_t size = gsf_input_size(input);
    if (name)
    {
        cout << "name = " << name << " (size: " << size << ")" << endl;
    }

    if (!is_dir)
        return;

    for (int i = 0; i < level; ++i)
        printf("   ");

    puts ("{");
    for (int i = 0 ; i < child_count; ++i)
    {
        GsfInput* child = gsf_infile_child_by_index (GSF_INFILE (input), i);
        list_content(child, level+1);
        g_object_unref(G_OBJECT(child));
    }

    for (int i = 0; i < level; ++i)
        printf("   ");
    puts ("}");
}

}

#endif

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
