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

#ifndef __ORCUS_MODEL_ODSTABLE_HPP__
#define __ORCUS_MODEL_ODSTABLE_HPP__

#include "model/global.hpp"

#include <string>
#include <unordered_map>

namespace orcus { namespace model {

class ods_table
{
public:
    typedef ::std::unordered_map<col_t, ::std::string>   row_type;
    typedef ::std::unordered_map<row_t, row_type*>       sheet_type;

    ods_table(const ::std::string& name);
    ~ods_table();

    const ::std::string& get_name() const;
    void set_cell(row_t row, col_t col, const ::std::string& val);
    ::std::string get_cell(row_t row, col_t col) const;
    size_t row_size() const;
    size_t col_size() const;

private:
    ods_table(); // disabled

private:
    ::std::string   m_name;
    sheet_type      m_sheet;
};

}}

#endif
