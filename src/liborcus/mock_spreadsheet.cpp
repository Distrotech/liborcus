/*************************************************************************
 *
 * Copyright (c) 2011-2012 Markus Mohrhard
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

#include "mock_spreadsheet.hpp"

#include <cassert>

using namespace orcus::spreadsheet;

namespace orcus { namespace spreadsheet { namespace mock {

//import_factory

import_factory::~import_factory()
{
}

orcus::spreadsheet::iface::import_shared_strings* import_factory::get_shared_strings()
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_styles* import_factory::get_styles()
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_sheet* import_factory::append_sheet(const char*, size_t)
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_sheet* import_factory::get_sheet(const char*, size_t)
{
    assert(false);
    return NULL;
}

// import_shared_strings

import_shared_strings::~import_shared_strings()
{
}

size_t import_shared_strings::append(const char*, size_t)
{
    assert(false);
    return 0;
}

size_t import_shared_strings::add(const char*, size_t)
{
    assert(false);
    return 0;
}

void import_shared_strings::set_segment_bold(bool)
{
    assert(false);
}

void import_shared_strings::set_segment_italic(bool)
{
    assert(false);
}

void import_shared_strings::set_segment_font_name(const char*, size_t)
{
    assert(false);
}

void import_shared_strings::set_segment_font_size(double)
{
    assert(false);
}

void import_shared_strings::append_segment(const char*, size_t)
{
    assert(false);
}

size_t import_shared_strings::commit_segments()
{
    assert(false);
    return 0;
}


// import_sheet

import_sheet::~import_sheet()
{
}

void import_sheet::set_auto(row_t, col_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_value(row_t, col_t, double)
{
    assert(false);
}

void import_sheet::set_bool(row_t, col_t, bool)
{
    assert(false);
}

void import_sheet::set_string(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_format(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_formula(row_t, col_t, formula_grammar_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, formula_grammar_t, size_t,
        const char*, size_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, formula_grammar_t, size_t,
        const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_array_formula(
    row_t, col_t, formula_grammar_t, const char*, size_t, row_t, col_t)
{
    assert(false);
}

void import_sheet::set_array_formula(
    row_t, col_t, formula_grammar_t, const char*, size_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_formula_result(row_t, col_t, const char*, size_t)
{
    assert(false);
}


}}}
