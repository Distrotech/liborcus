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

#include "orcus/xml_structure_tree.hpp"
#include "orcus/sax_parser.hpp"

namespace orcus {

namespace {

class xml_sax_handler
{
public:
    xml_sax_handler() {}

    void declaration() {}

    void start_element(const sax_parser_element& elem) {}

    void end_element(const sax_parser_element& elem) {}

    void characters(const pstring& val) {}

    void attribute(const pstring& ns, const pstring& name, const pstring& val) {}
};

}

struct xml_structure_tree_impl
{
    xml_structure_tree::element m_root;
};

xml_structure_tree::element::element() : repeat(false) {}

xml_structure_tree::xml_structure_tree() : mp_impl(new xml_structure_tree_impl) {}
xml_structure_tree::~xml_structure_tree()
{
    delete mp_impl;
}

void xml_structure_tree::read_file(const char* filepath)
{
}

}
