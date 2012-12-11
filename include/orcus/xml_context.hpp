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

#ifndef __ORCUS_XMLCONTEXT_HPP__
#define __ORCUS_XMLCONTEXT_HPP__

#include "xml_handler.hpp"

namespace orcus {

class tokens;

typedef ::std::pair<xmlns_token_t, xml_token_t> xml_token_pair_t;
typedef ::std::vector<xml_token_pair_t>         xml_elem_stack_t;

class xml_context_base
{
public:
    xml_context_base(const tokens& tokens);
    virtual ~xml_context_base() = 0;

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const = 0;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const = 0;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child) = 0;

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const ::std::vector<xml_token_attr_t>& attrs) = 0;
    virtual bool end_element(xmlns_token_t ns, xml_token_t name) = 0;
    virtual void characters(const pstring& str) = 0;

protected:
    const tokens& get_tokens() const;
    xml_token_pair_t push_stack(xmlns_token_t ns, xml_token_t name);
    bool pop_stack(xmlns_token_t ns, xml_token_t name);
    xml_token_pair_t& get_current_element();
    const xml_token_pair_t& get_current_element() const;
    xml_token_pair_t& get_parent_element();
    const xml_token_pair_t& get_parent_element() const;
    void warn_unhandled() const;
    void warn_unexpected() const;
    void warn(const char* msg) const;

    void set_default_ns(xmlns_token_t ns);
    xmlns_token_t get_default_ns() const;

    /**
     * Check if observed element equals expected element.  If not, it throws an
     * xml_structure_error exception.
     *
     * @param elem element observed.
     * @param ns namespace of expected element.
     * @param name name of expected element.
     * @param error custom error message if needed.
     */
    void xml_element_expected(
        const xml_token_pair_t& elem, xmlns_token_t ns, xml_token_t name,
        const ::std::string* error = NULL);

    void xml_element_expected(
        const xml_token_pair_t& elem, const xml_elem_stack_t& expected_elems);

private:
    const tokens& m_tokens;
    xml_elem_stack_t m_stack;
    xmlns_token_t m_default_ns; /// default namespace for worksheet element context.
};


}

#endif
