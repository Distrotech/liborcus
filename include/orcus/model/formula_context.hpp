/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#ifndef __ORCUS_MODEL_FORMULA_CONTEXT_HPP__
#define __ORCUS_MODEL_FORMULA_CONTEXT_HPP__

#include <ixion/interface/model_context.hpp>

namespace orcus { namespace model {

class document;

class formula_context : public ixion::interface::model_context
{
public:
    formula_context(document& doc);
    virtual ~formula_context();

    virtual const ixion::config& get_config() const;
    virtual const ixion::formula_name_resolver& get_name_resolver() const;
    virtual const ixion::base_cell* get_cell(const ixion::abs_address_t& addr) const;
    virtual ixion::base_cell* get_cell(const ixion::abs_address_t& addr);
    virtual ixion::interface::cells_in_range* get_cells_in_range(const ixion::abs_range_t& range);
    virtual ixion::interface::const_cells_in_range* get_cells_in_range(const ixion::abs_range_t& range) const;
    virtual std::string get_cell_name(const ixion::base_cell* p) const;
    virtual ixion::abs_address_t get_cell_position(const ixion::base_cell* p) const;
    virtual const ixion::formula_cell* get_named_expression(const ::std::string& name) const;
    virtual const std::string* get_named_expression_name(const ixion::formula_cell* expr) const;
    virtual ixion::matrix get_range_value(const ixion::abs_range_t& range) const;
    virtual ixion::interface::session_handler* get_session_handler() const;
    virtual const ixion::formula_tokens_t* get_formula_tokens(ixion::sheet_t sheet, size_t identifier) const;
    virtual size_t add_formula_tokens(ixion::sheet_t sheet, ixion::formula_tokens_t* p);
    virtual void remove_formula_tokens(ixion::sheet_t sheet, size_t identifier);
    virtual size_t add_string(const char* p, size_t n);
    virtual const std::string* get_string(size_t identifier) const;

private:
    model::document& m_doc;
    ixion::config* mp_config;
    ixion::formula_name_resolver* mp_name_resolver;
};

}}

#endif
