/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#ifndef __ORCUS_OPC_READER_HPP__
#define __ORCUS_OPC_READER_HPP__

#include "ooxml_schemas.hpp"
#include "xml_simple_stream_handler.hpp"
#include "orcus/env.hpp"

#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

struct zip;
struct zip_file;

namespace orcus {

class pstring;
class xmlns_repository;
struct opc_rel_extra;

/**
 * Class to handle parsing through all xml parts stored in a file packaged
 * according to the Open Package Convention (OPC).
 */
class opc_reader : boost::noncopyable
{
    typedef std::vector<std::string> dir_stack_type;

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
            schema_t type, const std::string& dir_path, const std::string& file_name, const opc_rel_extra* data) = 0;
    };

    struct zip_stream
    {
        std::vector<char> buffer;
        int buffer_read;
        struct ::zip_file* zfd;
    };

    opc_reader(xmlns_repository& ns_repo, part_handler& handler);

    void read_file(const char* fpath);
    bool open_zip_stream(const std::string& path, zip_stream& data);
    void close_zip_stream(zip_stream& data);

    /**
     * Read an xml part inside package.  The path is relative to the relation
     * file.
     *
     * @param path the path to the xml part.
     * @param type schema type.
     */
    void read_part(const pstring& path, const schema_t type, const opc_rel_extra* data);

    /**
     * Check if a relation file exists for a given xml part, and if it does,
     * read and process it.
     *
     * @param file_name name of the current xml part.
     * @param extras optional extra data file for client code to pass on to
     *               the next xml part(s).
     */
    void check_relation_part(const std::string& file_name, const opc_rel_extras_t* extras);

private:

    void list_content() const;
    void read_content();
    void read_content_types();
    void read_relations(const char* path, std::vector<opc_rel_t>& rels);

    std::string get_current_dir() const;

private:
    xmlns_repository& m_ns_repo;
    part_handler& m_handler;

    struct zip* m_archive;

    xml_simple_stream_handler m_opc_rel_handler;

    std::vector<xml_part_t> m_parts;
    std::vector<xml_part_t> m_ext_defaults;
    dir_stack_type m_dir_stack;
};

}

#endif
