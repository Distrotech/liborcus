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

#ifndef __ORCUS_SAX_HPP__
#define __ORCUS_SAX_HPP__

#include <iostream>

namespace orcus {

/** 
 * Template-based sax parser that doesn't use function pointer for 
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Char, typename _Handler>
class sax_parser
{
public:
    typedef _Char       char_type;
    typedef _Handler    handler_type;

    sax_parser(const char_type* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:
    const char_type* m_content;
    const size_t m_size;
    size_t m_pos;
    handler_type& m_handler;
};

template<typename _Char, typename _Handler>
sax_parser<_Char,_Handler>::sax_parser(const char_type* content, const size_t size, handler_type& handler) :
    m_content(content), m_size(size), m_pos(0), m_handler(handler)
{
}

template<typename _Char, typename _Handler>
sax_parser<_Char,_Handler>::~sax_parser()
{
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::parse()
{
    using namespace std;

    for (m_pos = 0; m_pos < m_size; ++m_pos)
    {
        const char_type c = m_content[m_pos];
        cout << c;
    }
    cout << endl;
    cout << "finished parsing" << endl;
}

}

#endif
