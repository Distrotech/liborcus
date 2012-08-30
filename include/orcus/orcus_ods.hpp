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

#ifndef __ORCUS_ORCUS_ODS_HPP__
#define __ORCUS_ORCUS_ODS_HPP__

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"

#include <boost/noncopyable.hpp>

struct zip;

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

class ORCUS_DLLPUBLIC orcus_ods
{
    orcus_ods(const orcus_ods&); // disabled
public:
    orcus_ods(spreadsheet::iface::import_factory* factory);
    ~orcus_ods();

    void read_file(const char* fpath);

private:
    void list_content(struct zip* archive) const;
    void read_content(struct zip* archive);
    void read_content_xml(const char* p, size_t size);

private:
    spreadsheet::iface::import_factory* mp_factory;
};

}

#endif
