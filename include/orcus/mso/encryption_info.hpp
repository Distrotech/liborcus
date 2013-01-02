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

#ifndef __ORCUS_MSO_ENCRYPTION_INFO_HPP__
#define __ORCUS_MSO_ENCRYPTION_INFO_HPP__

#include "../env.hpp"
#include <cstdlib>

namespace orcus { namespace mso {

struct encryption_info_reader_impl;

class ORCUS_DLLPUBLIC encryption_info_reader
{
    encryption_info_reader(const encryption_info_reader&); // disabled
    encryption_info_reader& operator= (const encryption_info_reader&); // disabled

public:
    encryption_info_reader();
    ~encryption_info_reader();

    void read(const char* p, size_t n);

private:
    encryption_info_reader_impl* mp_impl;
};

}}

#endif
