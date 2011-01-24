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

#ifndef __ORCUS_XLSX_CONTEXT_HPP__
#define __ORCUS_XLSX_CONTEXT_HPP__

#include "orcus/xml_context.hpp"
#include "orcus/ooxml/xlsx_types.hpp"

#include <unordered_map>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

namespace model {
    class sheet;
    class shared_strings_base;
}

/**
 * Context for xl/workbook.xml.
 */
class xlsx_workbook_context : public xml_context_base
{
public:
    typedef ::std::unordered_map<
        pstring, xlsx_rel_sheet_info, pstring::hash> sheet_info_type;

    xlsx_workbook_context(const tokens& tokens);
    virtual ~xlsx_workbook_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

    void pop_sheet_info(opc_rel_extras_t& sheets);

private:
    opc_rel_extras_t m_sheet_info;
    ::boost::ptr_vector<xlsx_rel_sheet_info> m_sheets;
};

/**
 * Top-level context for xl/worksheets/sheet<num>.xml.
 */
class xlsx_sheet_xml_context : public xml_context_base
{
public:
    xlsx_sheet_xml_context(const tokens& tokens);
    virtual ~xlsx_sheet_xml_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

private:
    model::sheet* mp_sheet; /// sheet model instance for the loaded document.
};

/**
 * Context for xl/sharedStrings.xml part.
 */
class xlsx_shared_strings_context : public xml_context_base
{
public:
    xlsx_shared_strings_context(const tokens& tokens, model::shared_strings_base* strings);
    virtual ~xlsx_shared_strings_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

private:
    model::shared_strings_base* mp_strings;
    pstring m_current_str;
};

}

#endif
