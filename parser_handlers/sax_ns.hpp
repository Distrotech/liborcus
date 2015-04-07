/*************************************************************************
 *
 * Copyright (c) 2015 Kohei Yoshida
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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_SAX_NS_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_SAX_NS_HPP

namespace orcus {

struct sax_ns_parser_attribute;
struct sax_ns_parser_element;

namespace sax {

struct doctype_declaration;

}}

/**
 * Template handler for sax_ns_parser.  Feel free to copy this as a starting
 * point for your own handler.
 */
class sax_ns_handler
{
public:
    void doctype(const orcus::sax::doctype_declaration& /*dtd*/) {}

    void start_declaration(const orcus::pstring& /*decl*/) {}

    void end_declaration(const orcus::pstring& /*decl*/) {}

    void start_element(const orcus::sax_ns_parser_element& /*elem*/) {}

    void end_element(const orcus::sax_ns_parser_element& /*elem*/) {}

    void characters(const orcus::pstring& /*val*/, bool /*transient*/) {}

    void attribute(const orcus::pstring& /*name*/, const orcus::pstring& /*val*/) {}

    void attribute(const orcus::sax_ns_parser_attribute& /*attr*/) {}
};

#endif
