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

#include "orcus/model/formula_context.hpp"

#include <ixion/config.hpp>
#include <ixion/formula_name_resolver.hpp>
#include <ixion/matrix.hpp>

namespace orcus { namespace model {

formula_context::formula_context(document& doc) :
    m_doc(doc),
    mp_config(new ixion::config),
    mp_name_resolver(new ixion::formula_name_resolver_a1) {}

formula_context::~formula_context()
{
    delete mp_config;
    delete mp_name_resolver;
}

const ixion::config& formula_context::get_config() const
{
    return *mp_config;
}

const ixion::formula_name_resolver& formula_context::get_name_resolver() const
{
    return *mp_name_resolver;
}

const ixion::base_cell* formula_context::get_cell(const ixion::abs_address_t& addr) const
{
    return NULL;
}

ixion::base_cell* formula_context::get_cell(const ixion::abs_address_t& addr)
{
    return NULL;
}

ixion::interface::cells_in_range*
formula_context::get_cells_in_range(const ixion::abs_range_t& range) const
{
    return NULL;
}

std::string formula_context::get_cell_name(const ixion::base_cell* p) const
{
    return std::string();
}

ixion::abs_address_t formula_context::get_cell_position(const ixion::base_cell* p) const
{
    return ixion::abs_address_t();
}

const ixion::formula_cell* formula_context::get_named_expression(const std::string& name) const
{
    return NULL;
}

const std::string* formula_context::get_named_expression_name(const ixion::formula_cell* expr) const
{
    return NULL;
}

void formula_context::get_cells(const ixion::abs_range_t& range, std::vector<ixion::base_cell*>& cells)
{
}

ixion::matrix formula_context::get_range_value(const ixion::abs_range_t& range) const
{
    return ixion::matrix(0, 0);
}

ixion::interface::session_handler* formula_context::get_session_handler() const
{
    return NULL;
}

ixion::formula_tokens_t* formula_context::get_formula_tokens(ixion::sheet_t sheet, size_t identifier)
{
    return NULL;
}

const ixion::formula_tokens_t* formula_context::get_formula_tokens(ixion::sheet_t sheet, size_t identifier) const
{
    return NULL;
}

size_t formula_context::add_formula_tokens(ixion::sheet_t sheet, ixion::formula_tokens_t* p)
{
    return 0;
}

void formula_context::remove_formula_tokens(ixion::sheet_t sheet, size_t identifier)
{

}

size_t formula_context::add_string(const char* p, size_t n)
{
    return 0;
}

const std::string* formula_context::get_string(size_t identifier) const
{
    return NULL;
}

}}

