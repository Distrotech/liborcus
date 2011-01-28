/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include <exception>
#include <string>
#include <cstdint>

#include "orcus/types.hpp"

namespace orcus {

class tokens;

class general_error : public ::std::exception
{
public:
    explicit general_error(const ::std::string& msg);
    virtual ~general_error() throw();
    virtual const char* what() const throw();
private:
    ::std::string m_msg;
};

class xml_structure_error : public general_error
{
public:
    explicit xml_structure_error(const ::std::string& msg);
    virtual ~xml_structure_error() throw();
};

// ============================================================================

void print_element(xmlns_token_t ns, xml_token_t name);

/**
 * Print attributes to stdout for debugging purposes. 
 */
void print_attrs(const tokens& tokens, const xml_attrs_t& attrs);

/**
 * Function object for deleting objects that are stored in map container as
 * pointers.
 */
template<typename T>
struct delete_map_object : public ::std::unary_function<typename T::value_type, void>
{
    void operator() (typename T::value_type& v)
    {
        delete v.second;
    }
};

inline ::std::ostream& operator<< (::std::ostream& os, const uint8_t val)
{
    return os << static_cast<const unsigned int>(val);
}

}

#endif
