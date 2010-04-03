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

#include "xmlhandler.hpp"
#include "tokens.hpp"
#include "global.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

void print_stack(const xml_elem_stack_t& elem_stack)
{
    cerr << "[ ";
    xml_elem_stack_t::const_iterator itr, itr_beg = elem_stack.begin(), itr_end = elem_stack.end();
    for (itr = itr_beg; itr != itr_end; ++itr)
    {
        if (itr != itr_beg)
            cerr << " -> ";
        cerr << tokens::get_nstoken_name(itr->first) << ":" << tokens::get_token_name(itr->second);
    }
    cerr << " ]";
}

}

void warn_unhandled(const xml_elem_stack_t& elem_stack)
{
    cerr << "warning: unhandled element ";
    print_stack(elem_stack);
    cerr << endl;
}

void warn_unexpected(const xml_elem_stack_t& elem_stack)
{
    cerr << "warning: unexpected element ";
    print_stack(elem_stack);
    cerr << endl;
}

xml_token_pair_t get_parent(const xml_elem_stack_t& elem_stack)
{
    if (elem_stack.size() < 2)
        throw general_error("attempt to get parent token when the stack size is less than 2");

    xml_elem_stack_t::const_reverse_iterator itr = elem_stack.rbegin() + 1;
    return *itr;
}

xml_stream_handler::xml_stream_handler()
{
}

xml_stream_handler::~xml_stream_handler()
{
}

}
