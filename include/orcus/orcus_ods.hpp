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

#ifndef ORCUS_ORCUS_ODS_HPP
#define ORCUS_ORCUS_ODS_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"
#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct orcus_ods_impl;
class zip_archive;

class ORCUS_DLLPUBLIC orcus_ods : public iface::import_filter
{
    orcus_ods(const orcus_ods&); // disabled
    orcus_ods& operator= (const orcus_ods&); // disabled

public:
    orcus_ods(spreadsheet::iface::import_factory* factory);
    ~orcus_ods();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:
    static void list_content(const zip_archive& archive);
    void read_content(const zip_archive& archive);
    void read_content_xml(const unsigned char* p, size_t size);

private:
    orcus_ods_impl* mp_impl;
};

}

#endif
