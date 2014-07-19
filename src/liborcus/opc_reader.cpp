/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "opc_reader.hpp"
#include "xml_stream_parser.hpp"

#include "ooxml_global.hpp"
#include "opc_context.hpp"
#include "ooxml_tokens.hpp"

#include "orcus/interface.hpp"

#include <iostream>
#include <boost/scoped_ptr.hpp>

using namespace std;

namespace orcus {

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

struct process_opc_rel : public unary_function<void, opc_rel_t>
{
    process_opc_rel(opc_reader& parent, opc_rel_extras_t* extras) :
        m_parent(parent), m_extras(extras) {}

    void operator() (opc_rel_t& v)
    {
        opc_rel_extra* data = NULL;
        if (m_extras)
        {
            opc_rel_extras_t::map_type::iterator it = m_extras->data.find(v.rid);
            if (it != m_extras->data.end())
                data = it->second;
        }
        m_parent.read_part(v.target, v.type, data);
    }
private:
    opc_reader& m_parent;
    opc_rel_extras_t* m_extras;
};

}

opc_reader::part_handler::~part_handler() {}

opc_reader::opc_reader(const iface::config& opt, xmlns_repository& ns_repo, session_context& cxt, part_handler& handler) :
    m_config(opt),
    m_ns_repo(ns_repo),
    m_session_cxt(cxt),
    m_handler(handler),
    m_opc_rel_handler(new opc_relations_context(m_session_cxt, opc_tokens)) {}

void opc_reader::read_file(const char* fpath)
{
    if (m_config.debug)
        cout << "reading " << fpath << endl;

    m_archive_stream.reset(new zip_archive_stream_fd(fpath));
    m_archive.reset(new zip_archive(m_archive_stream.get()));

    m_archive->load();

    m_dir_stack.push_back(string()); // push root directory.

    if (m_config.debug)
        list_content();
    read_content();

    m_archive.reset();
    m_archive_stream.reset();
}

bool opc_reader::open_zip_stream(const string& path, vector<unsigned char>& buf)
{
    return m_archive->read_file_entry(path.c_str(), buf);
}

void opc_reader::read_part(const pstring& path, const schema_t type, opc_rel_extra* data)
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

        if (!m_handler.handle_part(type, get_current_dir(), file_name, data))
        {
            if (m_config.debug)
            {
                cout << "---" << endl;
                cout << "unhandled relationship type: " << type << endl;
            }
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

void opc_reader::check_relation_part(const std::string& file_name, opc_rel_extras_t* extra)
{
    // Read the relationship file associated with this file, located at
    // _rels/<file name>.rels.
    vector<opc_rel_t> rels;
    m_dir_stack.push_back(string("_rels/"));
    string rels_file_name = file_name + ".rels";
    read_relations(rels_file_name.c_str(), rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, extra));
}

void opc_reader::list_content() const
{
    size_t num = m_archive->get_file_entry_count();
    cout << "number of files this archive contains: " << num << endl;

    for (size_t i = 0; i < num; ++i)
    {
        pstring filename = m_archive->get_file_entry_name(i);
        cout << filename << endl;
    }
}

void opc_reader::read_content()
{
    if (m_dir_stack.empty())
        return;

    // [Content_Types].xml

    read_content_types();
    if (m_config.debug)
    {
        for_each(m_parts.begin(), m_parts.end(), print_xml_content_types("part name"));
        for_each(m_ext_defaults.begin(), m_ext_defaults.end(), print_xml_content_types("extension default"));
    }

    // _rels/.rels

    m_dir_stack.push_back(string("_rels/"));
    vector<opc_rel_t> rels;
    read_relations(".rels", rels);
    m_dir_stack.pop_back();

    if (m_config.debug)
        for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, NULL));
}

void opc_reader::read_content_types()
{
    string filepath("[Content_Types].xml");
    vector<unsigned char> buffer;
    if (!open_zip_stream(filepath, buffer))
        return;

    if (buffer.empty())
        return;

    xml_stream_parser parser(m_ns_repo, opc_tokens, reinterpret_cast<const char*>(&buffer[0]), buffer.size());
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new opc_content_types_context(m_session_cxt, opc_tokens)));
    parser.set_handler(handler.get());
    parser.parse();

    opc_content_types_context& context =
        static_cast<opc_content_types_context&>(handler->get_context());
    context.pop_parts(m_parts);
    context.pop_ext_defaults(m_ext_defaults);
}

void opc_reader::read_relations(const char* path, vector<opc_rel_t>& rels)
{
    string filepath = get_current_dir() + path;
    if (m_config.debug)
        cout << "file path: " << filepath << endl;

    vector<unsigned char> buffer;
    if (!open_zip_stream(filepath, buffer))
        return;

    if (buffer.empty())
        return;

    xml_stream_parser parser(m_ns_repo, opc_tokens, reinterpret_cast<const char*>(&buffer[0]), buffer.size());

    opc_relations_context& context =
        static_cast<opc_relations_context&>(m_opc_rel_handler.get_context());
    context.init();
    parser.set_handler(&m_opc_rel_handler);
    parser.parse();
    context.pop_rels(rels);
}

string opc_reader::get_current_dir() const
{
    string pwd;
    vector<string>::const_iterator itr = m_dir_stack.begin(), itr_end = m_dir_stack.end();
    for (; itr != itr_end; ++itr)
        pwd += *itr;
    return pwd;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
