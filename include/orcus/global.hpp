/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#ifndef __ORCUS_GLOBAL_HPP__
#define __ORCUS_GLOBAL_HPP__

#include "types.hpp"
#include "env.hpp"

#include <string>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

namespace orcus {

class tokens;

void print_element(xmlns_token_t ns, xml_token_t name);

/**
 * Print attributes to stdout for debugging purposes.
 */
void print_attrs(const tokens& tokens, const xml_attrs_t& attrs);

/**
 * Load the content of a file into a file stream.
 *
 * @param filepath file to open
 * @param strm content of the file
 */
ORCUS_DLLPUBLIC void load_file_content(const char* filepath, std::string& strm);

template<typename _T>
struct default_deleter : public std::unary_function<_T*, void>
{
    void operator() (_T* p)
    {
        delete p;
    }
};

/**
 * Function object for deleting objects that are stored in map container as
 * pointers.
 */
template<typename T>
struct map_object_deleter : public ::std::unary_function<typename T::value_type, void>
{
    void operator() (typename T::value_type& v)
    {
        delete v.second;
    }
};

template<typename _T, typename _Deleter = default_deleter<_T> >
class unique_ptr : public boost::interprocess::unique_ptr<_T, _Deleter>
{
public:
    unique_ptr(_T* p) : boost::interprocess::unique_ptr<_T, _Deleter>(p) {}
};

}

#endif
