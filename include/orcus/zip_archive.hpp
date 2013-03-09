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

#ifndef __ORCUS_ZIP_ARCHIVE_HPP__
#define __ORCUS_ZIP_ARCHIVE_HPP__

#include "env.hpp"
#include <cstdlib>

namespace orcus {

class zip_archive_impl;

class ORCUS_DLLPUBLIC zip_archive
{
    zip_archive_impl* mp_impl;

    zip_archive(); // disabled
    zip_archive(const zip_archive&); // disabled
    zip_archive& operator= (const zip_archive); // disabled

public:
    zip_archive(const char* filepath);
    ~zip_archive();

    void open();
    void read_file_entries();
    void dump_file_entry(size_t pos) const;
    size_t get_file_entry_count() const;
};

}

#endif
