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
#include "orcus/model/document.hpp"
#include "orcus/global.hpp"

#include <ixion/config.hpp>
#include <ixion/formula_name_resolver.hpp>
#include <ixion/cell_listener_tracker.hpp>
#include <ixion/matrix.hpp>

#include <sstream>

namespace orcus { namespace model {

formula_context::formula_context(document& doc) :
    m_doc(doc),
    mp_config(new ixion::config),
    mp_name_resolver(new ixion::formula_name_resolver_a1(this)),
    mp_cell_listener_tracker(new ixion::cell_listener_tracker(*this)) {}

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

ixion::cell_listener_tracker& formula_context::get_cell_listener_tracker()
{
    return *mp_cell_listener_tracker;
}

const ixion::formula_cell* formula_context::get_named_expression(const std::string& name) const
{
    std::ostringstream os;
    os << "formula_context::get_named_expression not implemented!";
    os << " (name: " << name << ")";
    throw general_error(os.str());
    return NULL;
}

const std::string* formula_context::get_named_expression_name(const ixion::formula_cell* expr) const
{
    throw general_error("formula_context::get_named_expression_name not implemented!");
    return NULL;
}

ixion::matrix formula_context::get_range_value(const ixion::abs_range_t& range) const
{
    return m_doc.get_range_value(range);
}

ixion::iface::session_handler* formula_context::get_session_handler() const
{
    return NULL;
}

const ixion::formula_tokens_t* formula_context::get_formula_tokens(ixion::sheet_t sheet, size_t identifier) const
{
    return m_doc.get_formula_tokens(sheet, identifier, false);
}

const ixion::formula_tokens_t* formula_context::get_shared_formula_tokens(ixion::sheet_t sheet, size_t identifier) const
{
    return m_doc.get_formula_tokens(sheet, identifier, true);
}

ixion::abs_range_t formula_context::get_shared_formula_range(ixion::sheet_t sheet, size_t identifier) const
{
    throw general_error("formula_context::get_shared_formula_range not implemented!");
}

size_t formula_context::add_string(const char* p, size_t n)
{
    throw general_error("formula_context::add_string not implemented!");
    return 0;
}

const std::string* formula_context::get_string(size_t identifier) const
{
    throw general_error("formula_context::get_string not implemented!");
    return NULL;
}

sheet_t formula_context::get_sheet_index(const char* p, size_t n) const
{
    pstring test(p, n);
    return m_doc.get_sheet_index(test);
}

std::string formula_context::get_sheet_name(sheet_t sheet) const
{
    return m_doc.get_sheet_name(sheet).str();
}

}}

