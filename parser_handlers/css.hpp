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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_CSS_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_CSS_HPP

#include <orcus/css_types.hpp>

/**
 * Skeleton handler for css_parser.  Feel free to copy this as a starting
 * point for your own CSS handler.
 */
class css_parser_handler
{
public:
    void at_rule_name(const char* /*p*/, size_t /*n*/) {}

    void simple_selector_type(const char* /*p*/, size_t /*n*/) {}

    void simple_selector_class(const char* /*p*/, size_t /*n*/) {}

    void simple_selector_pseudo_element(orcus::css::pseudo_element_t /*pe*/) {}

    void simple_selector_pseudo_class(orcus::css::pseudo_class_t /*pc*/) {}

    void simple_selector_id(const char* /*p*/, size_t /*n*/) {}

    void end_simple_selector() {}

    void end_selector() {}

    void combinator(orcus::css::combinator_t /*combinator*/) {}

    void property_name(const char* /*p*/, size_t /*n*/) {}

    void value(const char* /*p*/, size_t /*n*/) {}

    void rgb(uint8_t /*red*/ , uint8_t /*green*/ , uint8_t /*blue*/ ) {}

    void rgba(uint8_t /*red*/ , uint8_t /*green*/ , uint8_t /*blue*/ , double /*alpha*/ ) {}

    void hsl(uint8_t /*hue*/ , uint8_t /*sat*/ , uint8_t /*light*/ ) {}

    void hsla(uint8_t /*hue*/ , uint8_t /*sat*/ , uint8_t /*light*/ , double /*alpha*/ ) {}

    void url(const char* /*p*/, size_t /*n*/) {}

    void begin_parse() {}

    void end_parse() {}

    void begin_block() {}

    void end_block() {}

    void begin_property() {}

    void end_property() {}
};

#endif
