/*************************************************************************
 *
 * Copyright (c) 2011-2013 Kohei Yoshida
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

#ifndef ORCUS_ORCUS_XLSX_HPP
#define ORCUS_ORCUS_XLSX_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct xlsx_rel_sheet_info;
struct orcus_xlsx_impl;
class xlsx_opc_handler;

class ORCUS_DLLPUBLIC orcus_xlsx : public iface::import_filter
{
    friend class xlsx_opc_handler;

    orcus_xlsx(const orcus_xlsx&); // disabled
    orcus_xlsx& operator= (const orcus_xlsx&); // disabled

public:
    orcus_xlsx(spreadsheet::iface::import_factory* factory);
    ~orcus_xlsx();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

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
    orcus_xlsx_impl* mp_impl;
};

}

#endif
