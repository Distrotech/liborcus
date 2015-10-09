/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SAX_PARSER_HPP
#define ORCUS_SAX_PARSER_HPP

#include "sax_parser_base.hpp"

namespace orcus {

struct sax_parser_default_config
{
    /**
     * When true, the parser will throw an exception if the xml stream doesn't
     * begin with a <?xml..?> declaration. When false, the parser will keep
     * parsing regardless of whether or not the xml stream begins with a
     * <?xml..?> declaration.
     */
    static const bool strict_xml_declaration = true;
};

/**
 * Template-based sax parser that doesn't use function pointer for
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Handler, typename _Config = sax_parser_default_config>
class sax_parser : public sax::parser_base
{
public:
    typedef _Handler handler_type;
    typedef _Config config_type;

    sax_parser(const char* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:

    /**
     * Parse XML header that occurs at the beginning of every XML stream i.e.
     * <?xml version="..." encoding="..." ?>
     */
    void header();
    void body();
    void element();
    void element_open(const char* begin_pos);
    void element_close(const char* begin_pos);
    void special_tag();
    void declaration(const char* name_check);
    void cdata();
    void doctype();
    void characters();
    void attribute();

private:
    handler_type& m_handler;
};

template<typename _Handler, typename _Config>
sax_parser<_Handler,_Config>::sax_parser(
    const char* content, const size_t size, handler_type& handler) :
    sax::parser_base(content, size),
    m_handler(handler)
{
}

template<typename _Handler, typename _Config>
sax_parser<_Handler,_Config>::~sax_parser()
{
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::parse()
{
    m_nest_level = 0;
    mp_char = mp_begin;
    header();
    blank();
    body();

    assert(m_buffer_pos == 0);
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::header()
{
    // we don't handle multi byte encodings so we can just skip bom entry if exists.
    skip_bom();
    blank();
    if (!has_char() || cur_char() != '<')
        throw sax::malformed_xml_error("xml file must begin with '<'.");

    if (config_type::strict_xml_declaration)
    {
        if (next_char_checked() != '?')
            throw sax::malformed_xml_error("xml file must begin with '<?'.");

        declaration("xml");
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::body()
{
    while (has_char())
    {
        if (cur_char() == '<')
        {
            element();
            if (!m_root_elem_open)
                // Root element closed.  Stop parsing.
                return;
        }
        else if (m_nest_level)
            // Call characters only when in xml hierarchy.
            characters();
        else
            next();
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::element()
{
    assert(cur_char() == '<');
    const char* pos = mp_char;
    char c = next_char_checked();
    switch (c)
    {
        case '/':
            element_close(pos);
        break;
        case '!':
            special_tag();
        break;
        case '?':
            declaration(NULL);
        break;
        default:
            if (!is_alpha(c))
                throw sax::malformed_xml_error("expected an alphabet.");
            element_open(pos);
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::element_open(const char* begin_pos)
{
    assert(is_alpha(cur_char()));

    sax::parser_element elem;
    element_name(elem, begin_pos);

    while (true)
    {
        blank();
        char c = cur_char();
        if (c == '/')
        {
            // Self-closing element: <element/>
            if (next_char() != '>')
                throw sax::malformed_xml_error("expected '/>' to self-close the element.");
            next();
            elem.end_pos = mp_char;
            m_handler.start_element(elem);
            reset_buffer_pos();
            m_handler.end_element(elem);
#if ORCUS_DEBUG_SAX_PARSER
            cout << "element_open: ns='" << elem.ns << "', name='" << elem.name << "' (self-closing)" << endl;
#endif
            return;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            elem.end_pos = mp_char;
            nest_up();
            m_handler.start_element(elem);
            reset_buffer_pos();
#if ORCUS_DEBUG_SAX_PARSER
            cout << "element_open: ns='" << elem.ns << "', name='" << elem.name << "'" << endl;
#endif
            return;
        }
        else
            attribute();
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::element_close(const char* begin_pos)
{
    assert(cur_char() == '/');
    nest_down();
    next_check();
    sax::parser_element elem;
    element_name(elem, begin_pos);

    if (cur_char() != '>')
        throw sax::malformed_xml_error("expected '>' to close the element.");
    next();
    elem.end_pos = mp_char;

    m_handler.end_element(elem);
#if ORCUS_DEBUG_SAX_PARSER
    cout << "element_close: ns='" << elem.ns << "', name='" << elem.name << "'" << endl;
#endif
    if (!m_nest_level)
        m_root_elem_open = false;
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::special_tag()
{
    assert(cur_char() == '!');
    // This can be either <![CDATA, <!--, or <!DOCTYPE.
    size_t len = remains();
    if (len < 2)
        throw sax::malformed_xml_error("special tag too short.");

    switch (next_char())
    {
        case '-':
        {
            // Possibly comment.
            if (next_char() != '-')
                throw sax::malformed_xml_error("comment expected.");

            len -= 2;
            if (len < 3)
                throw sax::malformed_xml_error("malformed comment.");

            next();
            comment();
        }
        break;
        case '[':
        {
            // Possibly a CDATA.
            expects_next("CDATA[", 6);
            if (has_char())
                cdata();
        }
        break;
        case 'D':
        {
            // check if this is a DOCTYPE.
            expects_next("OCTYPE", 6);
            blank();
            if (has_char())
                doctype();
        }
        break;
        default:
            throw sax::malformed_xml_error("failed to parse special tag.");
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::declaration(const char* name_check)
{
    assert(cur_char() == '?');
    next_check();

    // Get the declaration name first.
    pstring decl_name;
    name(decl_name);
#if ORCUS_DEBUG_SAX_PARSER
    cout << "sax_parser::declaration: start name='" << decl_name << "'" << endl;
#endif

    if (name_check && decl_name != name_check)
    {
        std::ostringstream os;
        os << "declaration name of '" << name_check << "' was expected, but '" << decl_name << "' was found instead.";
        throw sax::malformed_xml_error(os.str());
    }

    m_handler.start_declaration(decl_name);
    blank();

    // Parse the attributes.
    while (cur_char_checked() != '?')
    {
        attribute();
        blank();
    }
    if (next_char_checked() != '>')
        throw sax::malformed_xml_error("declaration must end with '?>'.");

    m_handler.end_declaration(decl_name);
    reset_buffer_pos();
    next();
#if ORCUS_DEBUG_SAX_PARSER
    cout << "sax_parser::declaration: end name='" << decl_name << "'" << endl;
#endif
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::cdata()
{
    size_t len = remains();
    assert(len > 3);

    // Parse until we reach ']]>'.
    const char* p0 = mp_char;
    size_t i = 0, match = 0;
    for (char c = cur_char(); i < len; ++i, c = next_char())
    {
        if (c == ']')
        {
            // Be aware that we may encounter a series of more than two ']'
            // characters, in which case we'll only count the last two.

            if (match == 0)
                // First ']'
                ++match;
            else if (match == 1)
                // Second ']'
                ++match;
        }
        else if (c == '>' && match == 2)
        {
            // Found ']]>'.
            size_t cdata_len = i - 2;
            m_handler.characters(pstring(p0, cdata_len), false);
            next();
            return;
        }
        else
            match = 0;
    }
    throw sax::malformed_xml_error("malformed CDATA section.");
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::doctype()
{
    // Parse the root element first.
    sax::doctype_declaration param;
    name(param.root_element);
    blank();

    // Either PUBLIC or SYSTEM.
    size_t len = remains();
    if (len < 6)
        sax::malformed_xml_error("DOCTYPE section too short.");

    param.keyword = sax::doctype_declaration::keyword_type::dtd_private;
    char c = cur_char();
    if (c == 'P')
    {
        if (next_char() != 'U' || next_char() != 'B' || next_char() != 'L' || next_char() != 'I' || next_char() != 'C')
            throw sax::malformed_xml_error("malformed DOCTYPE section.");

        param.keyword = sax::doctype_declaration::keyword_type::dtd_public;
    }
    else if (c == 'S')
    {
        if (next_char() != 'Y' || next_char() != 'S' || next_char() != 'T' || next_char() != 'E' || next_char() != 'M')
            throw sax::malformed_xml_error("malformed DOCTYPE section.");
    }

    next_check();
    blank();
    has_char_throw("DOCTYPE section too short.");

    // Parse FPI.
    value(param.fpi, false);

    has_char_throw("DOCTYPE section too short.");
    blank();
    has_char_throw("DOCTYPE section too short.");

    if (cur_char() == '>')
    {
        // Optional URI not given. Exit.
#if ORCUS_DEBUG_SAX_PARSER
        cout << "sax_parser::doctype: root='" << param.root_element << "', fpi='" << param.fpi << "'" << endl;
#endif
        m_handler.doctype(param);
        next();
        return;
    }

    // Parse optional URI.
    value(param.uri, false);

    has_char_throw("DOCTYPE section too short.");
    blank();
    has_char_throw("DOCTYPE section too short.");

    if (cur_char() != '>')
        throw sax::malformed_xml_error("malformed DOCTYPE section - closing '>' expected but not found.");

#if ORCUS_DEBUG_SAX_PARSER
    cout << "sax_parser::doctype: root='" << param.root_element << "', fpi='" << param.fpi << "' uri='" << param.uri << "'" << endl;
#endif
    m_handler.doctype(param);
    next();
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::characters()
{
    const char* p0 = mp_char;
    for (; has_char(); next())
    {
        if (cur_char() == '<')
            break;

        if (cur_char() == '&')
        {
            // Text span with one or more encoded characters. Parse using cell buffer.
            cell_buffer& buf = get_cell_buffer();
            buf.reset();
            buf.append(p0, mp_char-p0);
            characters_with_encoded_char(buf);
            if (buf.empty())
                m_handler.characters(pstring(), false);
            else
                m_handler.characters(pstring(buf.get(), buf.size()), true);
            return;
        }
    }

    if (mp_char > p0)
    {
        pstring val(p0, mp_char-p0);
        m_handler.characters(val, false);
    }
}

template<typename _Handler, typename _Config>
void sax_parser<_Handler,_Config>::attribute()
{
    sax::parser_attribute attr;
    pstring attr_ns_name, attr_name, attr_value;
    attribute_name(attr.ns, attr.name);

#if ORCUS_DEBUG_SAX_PARSER
    std::ostringstream os;
    os << "sax_parser::attribute: ns='" << attr.ns << "', name='" << attr.name << "'";
#endif

    char c = cur_char();
    if (c != '=')
    {
        std::ostringstream os;
        os << "Attribute must begin with 'name=..'. (ns='" << attr.ns << "', name='" << attr.name << "')";
        throw sax::malformed_xml_error(os.str());
    }

    next_check();
    attr.transient = value(attr.value, true);
    if (attr.transient)
        // Value is stored in a temporary buffer. Push a new buffer.
        inc_buffer_pos();

#if ORCUS_DEBUG_SAX_PARSER
    os << " value='" << attr.value << "'" << endl;
    cout << os.str();
#endif

    m_handler.attribute(attr);
}

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
