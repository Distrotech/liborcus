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

#ifndef ORCUS_XML_CONTEXT_GLOBAL_HPP
#define ORCUS_XML_CONTEXT_GLOBAL_HPP

#include "orcus/types.hpp"
#include "orcus/pstring.hpp"

#include <functional>

namespace orcus {

class string_pool;

/**
 * Use this just to get the value of a single attribute for a given element.
 */
class single_attr_getter : public std::unary_function<xml_token_attr_t, void>
{
    string_pool& m_pool;
    pstring m_value;
    xml_token_t m_name;

public:
    single_attr_getter(string_pool& pool, xml_token_t name);
    void operator() (const xml_token_attr_t& attr);
    pstring get_value() const;
};

}

#endif
