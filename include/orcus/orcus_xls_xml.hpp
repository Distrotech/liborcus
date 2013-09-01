/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef ORCUS_ORCUS_XLS_XML_HPP
#define ORCUS_ORCUS_XLS_XML_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct orcus_xls_xml_impl;

class ORCUS_DLLPUBLIC orcus_xls_xml
{
    orcus_xls_xml(const orcus_xls_xml&); // disabled
    orcus_xls_xml& operator= (const orcus_xls_xml&); // disabled

public:
    orcus_xls_xml(spreadsheet::iface::import_factory* factory);
    ~orcus_xls_xml();

    static bool detect(const unsigned char* blob, size_t size);

    void read_file(const char* fpath);

private:
    orcus_xls_xml_impl* mp_impl;
};

}

#endif
