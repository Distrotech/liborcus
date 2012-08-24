/*************************************************************************
 *
 * Copyright (c) 2011-2012 Kohei Yoshida
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

#ifndef __ORCUS_ORCUS_XLSX_HPP__
#define __ORCUS_ORCUS_XLSX_HPP__

#include "env.hpp"
#include "opc_reader.hpp"

#include <boost/noncopyable.hpp>

namespace orcus {

namespace spreadsheet { namespace iface { class factory; }}

struct xlsx_rel_sheet_info;

class ORCUS_DLLPUBLIC orcus_xlsx : public ::boost::noncopyable
{
    class opc_handler : public opc_reader::part_handler
    {
        orcus_xlsx& m_parent;
    public:
        opc_handler(orcus_xlsx& parent);
        virtual ~opc_handler();
        virtual bool handle_part(
            schema_t type, const std::string& dir_path, const std::string& file_name, const opc_rel_extra* data);
    };

public:
    orcus_xlsx(spreadsheet::iface::factory* factory);
    ~orcus_xlsx();

    void read_file(const char* fpath);

private:

    void read_workbook(const std::string& dir_path, const std::string& file_name);

    /**
     * Parse a sheet xml part that contains data stored in a single sheet.
     */
    void read_sheet(const std::string& dir_path, const std::string& file_name, const xlsx_rel_sheet_info* data);

    /**
     * Parse sharedStrings.xml part that contains a list of strings referenced
     * in the document.
     */
    void read_shared_strings(const std::string& dir_path, const std::string& file_name);

    void read_styles(const std::string& dir_path, const std::string& file_name);

private:
    spreadsheet::iface::factory* mp_factory;
    opc_handler m_opc_handler;
    opc_reader m_opc_reader;
};

}

#endif
