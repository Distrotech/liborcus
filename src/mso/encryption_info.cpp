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

#include "orcus/mso/encryption_info.hpp"

#define ORCUS_DEBUG_MSO_ENCRYPTION_INFO 1

#if ORCUS_DEBUG_MSO_ENCRYPTION_INFO
#include <iostream>
#endif

using namespace std;

namespace orcus { namespace mso {

struct encryption_info_reader_impl
{
};

encryption_info_reader::encryption_info_reader() :
    mp_impl(new encryption_info_reader_impl) {}

encryption_info_reader::~encryption_info_reader()
{
    delete mp_impl;
}

void encryption_info_reader::read(const char* p, size_t n)
{
#if ORCUS_DEBUG_MSO_ENCRYPTION_INFO
    cout << "encryption_info_reader::read: stream size=" << n << endl;
#endif
}

}}
