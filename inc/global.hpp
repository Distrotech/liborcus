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

#include <pthread.h>
#include <exception>
#include <string>

#include "types.hpp"

namespace orcus {

class tokens_base;

class thread_mutex_guard
{
public:
    explicit thread_mutex_guard(pthread_mutex_t& lock);
    ~thread_mutex_guard();
private:
    pthread_mutex_t& m_lock;
};

// ============================================================================

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
void print_attrs(const tokens_base& tokens, const xml_attrs_t& attrs);

}

#endif
