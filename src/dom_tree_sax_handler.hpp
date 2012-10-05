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

#ifndef __ORCUS_DOM_TREE_SAX_HANDLER_HPP__
#define __ORCUS_DOM_TREE_SAX_HANDLER_HPP__

#include "orcus/dom_tree.hpp"

#include <ostream>

namespace orcus {

struct sax_parser_element;
class pstring;

/**
 * Sax handler just to wrap a dom tree instance.
 */
class dom_tree_sax_handler
{
    dom_tree m_tree;
public:
    void declaration();
    void start_element(const sax_parser_element& elem);
    void end_element(const sax_parser_element& elem);
    void characters(const pstring& val);
    void attribute(const pstring& ns, const pstring& name, const pstring& val);
    void dump_compact(std::ostream& os);
};

}

#endif
