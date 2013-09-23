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

#ifndef ORCUS_ORCUS_GNUMERIC_HPP
#define ORCUS_ORCUS_GNUMERIC_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct orcus_gnumeric_impl;

class ORCUS_DLLPUBLIC orcus_gnumeric : public iface::import_filter
{
    orcus_gnumeric(const orcus_gnumeric&); // disabled
public:
    orcus_gnumeric(spreadsheet::iface::import_factory* factory);
    ~orcus_gnumeric();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:
    void read_content_xml(const char* p, size_t size);

private:
    orcus_gnumeric_impl* mp_impl;
};

}

#endif
