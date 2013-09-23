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

#ifndef ORCUS_ORCUS_CSV_HPP
#define ORCUS_ORCUS_CSV_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_factory;
}}

class ORCUS_DLLPUBLIC orcus_csv : public iface::import_filter
{
    orcus_csv(const orcus_csv&); // disabled
    orcus_csv& operator=(const orcus_csv&); // disabled

public:
    orcus_csv(spreadsheet::iface::import_factory* factory);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:
    void parse(const std::string& strm);

private:
    spreadsheet::iface::import_factory* mp_factory;
};

}

#endif
