/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_OPC_READER_HPP
#define INCLUDED_ORCUS_OPC_READER_HPP

#include "orcus/env.hpp"
#include "orcus/zip_archive.hpp"
#include "orcus/zip_archive_stream.hpp"

#include "ooxml_schemas.hpp"
#include "xml_simple_stream_handler.hpp"

#include <vector>
#include <string>
#include <unordered_set>

namespace orcus {

struct config;

class xmlns_repository;
struct session_context;
struct opc_rel_extra;

/**
 * Class to handle parsing through all xml parts stored in a file packaged
 * according to the Open Package Convention (OPC).
 */
class opc_reader
{
    typedef std::vector<std::string> dir_stack_type;
    typedef std::unordered_set<std::string> part_set_type;

    opc_reader(const opc_reader&) = delete;
    opc_reader& operator=(const opc_reader&) = delete;

public:
    /**
     * Interface class for the user of opc_reader to receive callback to
     * handle each xml part.
     */
    class part_handler
    {
    public:
        virtual ~part_handler() = 0;

        /**
         * Client code needs to implement this method to handle each xml part.
         *
         * @param type schema type signifying the content type stored in this
         *             part.
         * @param dir_path directory path relative to package root.
         * @param file_name name of the xml part without the directory path.
         * @param data extra data passed on from the client code.
         *
         * @return true if handled, false if not handled.
         */
        virtual bool handle_part(
            schema_t type, const std::string& dir_path, const std::string& file_name, opc_rel_extra* data) = 0;
    };

    opc_reader(const config& opt, xmlns_repository& ns_repo, session_context& session_cxt, part_handler& handler);

    void read_file(std::unique_ptr<zip_archive_stream>&& stream);
    bool open_zip_stream(const std::string& path, std::vector<unsigned char>& buf);

    /**
     * Read an xml part inside package.  The path is relative to the relation
     * file.
     *
     * @param path the path to the xml part.
     * @param type schema type.
     */
    void read_part(const pstring& path, const schema_t type, opc_rel_extra* data);

    /**
     * Check if a relation file exists for a given xml part, and if it does,
     * read and process it.
     *
     * @param file_name name of the current xml part.
     * @param extras optional extra data file for client code to pass on to
     *               the next xml part(s).
     */
    void check_relation_part(const std::string& file_name, opc_rel_extras_t* extras);

private:

    void list_content() const;
    void read_content();
    void read_content_types();
    void read_relations(const char* path, std::vector<opc_rel_t>& rels);

    std::string get_current_dir() const;

private:
    const config& m_config;
    xmlns_repository& m_ns_repo;
    session_context& m_session_cxt;
    part_handler& m_handler;

    std::unique_ptr<zip_archive> m_archive;
    std::unique_ptr<zip_archive_stream> m_archive_stream;

    xml_simple_stream_handler m_opc_rel_handler;

    std::vector<xml_part_t> m_parts;
    std::vector<xml_part_t> m_ext_defaults;
    dir_stack_type m_dir_stack;
    part_set_type m_handled_parts;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
