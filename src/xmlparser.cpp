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

#include "xmlparser.hpp"
#include "xmlhandler.hpp"
#include "tokens.hpp"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

struct parser_context
{
    xml_stream_parser& parser;

    explicit parser_context(xml_stream_parser& p) : parser(p) {}
};

void name_to_tokens(const xmlChar* name, xmlns_token_t& nstoken, xml_token_t& token)
{
    nstoken = XMLNS_UNKNOWN_TOKEN;
    token = XML_UNKNOWN_TOKEN;

    vector<char> buffer;
    buffer.reserve(100);
    size_t i = 0;
    char c = name[i++];
    bool in_ns = true;

    while (c)
    {
        if (c == ':')
        {
            if (buffer.empty())
                throw xml_stream_parser::parse_error("empty namespace");

            buffer.push_back(0); // null-terminate.
            nstoken = tokens::get_nstoken(&buffer[0]);
            if (!tokens::is_valid_nstoken(nstoken))
            {
                ostringstream os;
                os << "invalid namespace: " << &buffer[0];
                throw xml_stream_parser::parse_error(os.str());
            }
            buffer.clear();
            in_ns = false;
        }
        else
            buffer.push_back(c);

        c = name[i++];
    }

    if (in_ns)
    {    
        ostringstream os;
        os << "element name is not properly namespaced: " << name;
        throw xml_stream_parser::parse_error(os.str());
    }

    if (buffer.empty())
        throw xml_stream_parser::parse_error("empty element name");

    buffer.push_back(0); // null-terminate.
    token = tokens::get_token(&buffer[0]);
}

void start_document(void* ctx)
{
    cout << "start document" << endl;
}

void end_document(void* ctx)
{
    cout << "end document" << endl;
}

void start_element(void* ctx, const xmlChar* name, const xmlChar** attrs)
{
    cout << "start element: " << name << endl;
    xmlns_token_t nstoken;
    xml_token_t token;
    name_to_tokens(name, nstoken, token);
    cout << "check: namespace = " << tokens::get_nstoken_name(nstoken) << "  name = " << tokens::get_token_name(token) << endl;
}

void end_element(void* ctx, const xmlChar* name)
{
    cout << "end element: " << name << endl;
    xmlns_token_t nstoken;
    xml_token_t token;
    name_to_tokens(name, nstoken, token);
    cout << "check: namespace = " << tokens::get_nstoken_name(nstoken) << "  name = " << tokens::get_token_name(token) << endl;
}

void start_element_ns(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI,
    int nb_namespaces,
    const xmlChar **namespaces,
    int nb_attributes,
    int nb_defaulted,
    const xmlChar **attributes)
{
}

void end_element_ns(
    void *ctx,
    const xmlChar *localname,
    const xmlChar *prefix,
    const xmlChar *URI)
{
}

void characters(void* ctx, const xmlChar* ch, int len)
{
    cout << "chars: ";
    for (int i = 0; i < len; ++i)
        cout << ch[i];
    cout << endl;
}

xmlSAXHandler sax_handler_struct = {
    NULL, /* internalSubset */
    NULL, /* isStandalone */
    NULL, /* hasInternalSubset */
    NULL, /* hasExternalSubset */
    NULL, /* resolveEntity */
    NULL, /* getEntity */
    NULL, /* entityDecl */
    NULL, /* notationDecl */
    NULL, /* attributeDecl */
    NULL, /* elementDecl */
    NULL, /* unparsedEntityDecl */
    NULL, /* setDocumentLocator */
    start_document,
    end_document,
    start_element,
    end_element,
    NULL, /* reference */
    characters,
    NULL, /* ignorableWhitespace */
    NULL, /* processingInstruction */
    NULL, /* comment */
    NULL, /* xmlParserWarning */
    NULL, /* xmlParserError */
    NULL, /* xmlParserError */
    NULL, /* getParameterEntity */
    NULL, /* cdataBlock; */
    NULL, /* externalSubset; */
    1,
    NULL,
    start_element_ns,
    end_element_ns,
    NULL  /* xmlStructuredErrorFunc */
};

static xmlSAXHandlerPtr sax_handler = &sax_handler_struct;

}

// ============================================================================

xml_stream_parser::parse_error::parse_error(const string& msg) :
    m_msg(msg) {}

xml_stream_parser::parse_error::~parse_error() throw() {}

const char* xml_stream_parser::parse_error::what() const throw()
{
    return m_msg.c_str();
}

xml_stream_parser::xml_stream_parser(const uint8_t* content, size_t size, const string& name) :
    mp_handler(NULL), m_content(content), m_size(size), m_name(name)
{
}

xml_stream_parser::~xml_stream_parser()
{
}

void xml_stream_parser::parse()
{
    tokens::init();
    parser_context cxt(*this);
    xmlSAXUserParseMemory(sax_handler, &cxt, reinterpret_cast<const char*>(m_content), m_size);
}

void xml_stream_parser::set_handler(xml_stream_handler* handler)
{
    mp_handler = handler;
}

}
