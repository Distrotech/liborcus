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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_SAX_TOKEN_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_SAX_TOKEN_HPP

namespace orcus {

struct xml_token_element_t;

}

/**
 * Skeleton handler for sax_token_parser.  Feel free to copy this as a
 * starting point for your own handler.
 */
class sax_token_handler
{
public:
    /**
     * Called at the start of each element.
     *
     * @param elem struct containing the element's information as well as all
     *             the attributes that belong to the element.
     */
    void start_element(const orcus::xml_token_element_t& elem) {}

    /**
     * Called at the end of each element.
     *
     * @param elem struct containing the element's information as well as all
     *             the attributes that belong to the element.
     */
    void end_element(const orcus::xml_token_element_t& elem) {}

    /**
     * Called when a segment of a text content is parsed.  Each text content
     * is a direct child of an element, which may have multiple child contents
     * when the element also has a child element that are direct sibling to
     * the text contents or the text contents are splitted by a comment.
     *
     * @param val value of the text content.
     * @param transient when true, the text content has been converted and is
     *                  stored in a temporary buffer due to presence of one or
     *                  more encoded characters, in which case <em>the passed
     *                  text value needs to be either immediately converted to
     *                  a non-text value or be interned within the scope of
     *                  the callback</em>.
     */
    void characters(const orcus::pstring& val, bool transient) {}
};

#endif
