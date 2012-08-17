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

#ifndef __ORCUS_ORCUS_XML_HPP__
#define __ORCUS_ORCUS_XML_HPP__

#include "env.hpp"
#include "model/types.hpp"

namespace orcus {

class pstring;

namespace model { namespace iface {
    class factory;
}}

struct orcus_xml_impl;

class ORCUS_DLLPUBLIC orcus_xml
{
    orcus_xml(const orcus_xml&); // disabled

public:
    orcus_xml(model::iface::factory* factory);

    void set_cell_link(const pstring& xpath, const pstring& sheet, model::row_t row, model::col_t col);

    void start_range(const pstring& sheet, model::row_t row, model::col_t col);
    void append_field_link(const pstring& xpath);
    void commit_range();

    void append_sheet(const pstring& name);

    void read_map_file(const char* filepath);
    void read_file(const char* filepath);

private:
    orcus_xml_impl* mp_impl;
};

}

#endif
