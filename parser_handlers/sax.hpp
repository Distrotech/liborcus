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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_SAX_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_SAX_HPP

namespace orcus {

namespace sax {

struct doctype_declaration;
struct parser_element;

}

class pstring;

}

/**
 * Template handler for sax_parser.  Feel free to copy this as a starting
 * point for your own handler.
 */
class sax_parser_handler
{
public:
    /**
     * Called when a doctype declaration &lt;!DOCTYPE ... &gt; is encountered.
     *
     * @param param struct containing doctype declaration data.
     */
    void doctype(const orcus::sax::doctype_declaration& param) {}

    /**
     * Called when &lt;?... is encountered, where the '...' may be an
     * arbitraray dentifier.  One common declaration is &lt;?xml which is
     * typically given at the start of an XML stream.
     *
     * @param decl name of the identifier.
     */
    void start_declaration(const orcus::pstring& decl) {}

    /**
     * Called when the closing tag (&gt;) of a &lt;?... &gt; is encountered.
     *
     * @param decl name of the identifier.
     */
    void end_declaration(const orcus::pstring& decl) {}

    /**
     * Called at the start of each element.
     *
     * @param elem information of the element being parsed.
     */
    void start_element(const orcus::sax::parser_element& elem) {}

    /**
     * Called at the end of each element.
     *
     * @param elem information of the element being parsed.
     */
    void end_element(const orcus::sax::parser_element& elem) {}

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

    /**
     * Called upon parsing of a name-value pair within &lt;? ... &gt; tag.
     *
     * @param name attribute name.
     * @param val attribute value text.
     */
    void attribute(const orcus::pstring& name, const orcus::pstring& val) {}

    /**
     * Called upon parsing of an attribute of an element.  Note that <em>when
     * the attribute's transient flag is set, the attribute value is stored in
     * a temporary buffer due to presence of one or more encoded characters,
     * and must be processed within the scope of the callback</em>.
     *
     * @param attr struct containing attribute information.
     */
    void attribute(const orcus::sax::parser_attribute& attr) {}
};

#endif
