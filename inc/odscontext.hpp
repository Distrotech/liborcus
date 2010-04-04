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

#ifndef __ODSCONTEXT_HPP__
#define __ODSCONTEXT_HPP__

#include "xmlhandler.hpp"
#include "model/odstable.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

/** 
 * The role of this class is to interpret data passed on from the handler 
 * and build a document model.  In the future I will make an interface class 
 * above this class so that an external application can provide its own 
 * implementation in order to build its own document model. 
 */
class ods_content_xml_context
{
public:
    struct row_attr
    {
        uint32_t number_rows_repeated;
        row_attr();
    };

    ods_content_xml_context();
    ~ods_content_xml_context();

    void start_content();
    void end_content();

    void start_table(const xml_attrs_t& attrs);
    void end_table();

    void start_column(const xml_attrs_t& attrs);
    void end_column();

    void start_row(const xml_attrs_t& attrs);
    void end_row();

    void start_cell(const xml_attrs_t& attrs);
    void end_cell();

    void print_html(const ::std::string& filepath) const;

private:
    ::boost::ptr_vector<model::ods_table> m_tables;

    row_attr m_row_attr;

    uint32_t m_row;
    uint32_t m_col;
};

}

#endif
