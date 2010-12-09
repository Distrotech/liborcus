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

#ifndef __ORCUS_XMLPARSER_HPP__
#define __ORCUS_XMLPARSER_HPP__

#include <cstdint>
#include <cstdlib>
#include <string>
#include <exception>

namespace orcus {

class xml_stream_handler;
class tokens;

/** 
 * This class does NOT store the stream content which is just a pointer to 
 * the first char of the content stream.  Make sure you finish parsing while 
 * the content pointer is valid. 
 */
class xml_stream_parser
{
public:
    class parse_error : public ::std::exception
    {
    public:
        parse_error(const ::std::string& msg);
        virtual ~parse_error() throw();
        virtual const char* what() const throw();
    private:
        ::std::string m_msg;
    };

    xml_stream_parser(const tokens& tokens, const uint8_t* content, size_t size, const ::std::string& name);
    ~xml_stream_parser();

    void parse();

    void set_handler(xml_stream_handler* handler);
    xml_stream_handler* get_handler() const;

private:
    xml_stream_parser(); // disabled

    const tokens& m_tokens;
    xml_stream_handler* mp_handler;
    const uint8_t* m_content;
    size_t m_size;
    ::std::string m_name;  // stream name
};

}

#endif
