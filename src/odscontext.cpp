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

#include "odscontext.hpp"

#include <iostream>
#include <fstream>

using namespace std;

namespace orcus {

namespace {

class table_attr_parser : public unary_function<xml_attr, void>
{
public:
    table_attr_parser() {}
    table_attr_parser(const table_attr_parser& r) :
        m_name(r.m_name)
    {
    }

    void operator() (const xml_attr& attr)
    {
        if (attr.ns == XMLNS_table && attr.name == XML_name)
            m_name = attr.value;
    }

    model::ods_table* create_table() const
    {
        return new model::ods_table(m_name);
    }
private:
    ::std::string m_name;
};

class row_attr_parser : public unary_function<xml_attr, void>
{
public:
    row_attr_parser(ods_content_xml_context::row_attr& attr) :
        m_attr(attr) {}
    row_attr_parser(const row_attr_parser& r) :
        m_attr(r.m_attr) {}

    void operator() (const xml_attr& attr)
    {
        if (attr.ns == XMLNS_table && attr.name == XML_number_rows_repeated)
        {
            char* endptr;
            long val = strtol(attr.value.c_str(), &endptr, 10);
            if (endptr != attr.value.c_str())
                m_attr.number_rows_repeated = static_cast<uint32_t>(val);
        }
    }
private:
    ods_content_xml_context::row_attr& m_attr;
};

class cell_attr_parser : public unary_function<xml_attr, void>
{
public:
    cell_attr_parser(ods_content_xml_context::cell_attr& attr) :
        m_attr(attr) {}
    cell_attr_parser(const cell_attr_parser& r) :
        m_attr(r.m_attr) {}

    void operator() (const xml_attr& attr)
    {
        if (attr.ns == XMLNS_table && attr.name == XML_number_columns_repeated)
        {
            char* endptr;
            long val = strtol(attr.value.c_str(), &endptr, 10);
            if (endptr != attr.value.c_str())
                m_attr.number_columns_repeated = static_cast<uint32_t>(val);
        }
    }
private:
    ods_content_xml_context::cell_attr& m_attr;
};

class table_html_printer : public unary_function<model::ods_table, void>
{
public:
    table_html_printer(ofstream& file) : m_file(file) {}
    void operator() (const model::ods_table& table)
    {
        m_file 
            << "<table>" 
            << "<caption>" << table.get_name() << "</caption>"
            << "</table>" << endl;
    }
private:
    ofstream& m_file;
};

}

// ============================================================================

ods_content_xml_context::row_attr::row_attr() :
    number_rows_repeated(1)
{
}

ods_content_xml_context::cell_attr::cell_attr() :
    number_columns_repeated(1)
{
}

// ============================================================================

ods_content_xml_context::ods_content_xml_context() :
    m_row(0), m_col(0)
{
}

ods_content_xml_context::~ods_content_xml_context()
{
}

void ods_content_xml_context::start_content()
{
    cout << "start content" << endl;
}

void ods_content_xml_context::end_content()
{
    cout << "end content" << endl;
}

void ods_content_xml_context::start_table(const xml_attrs_t& attrs)
{
    table_attr_parser parser = for_each(attrs.begin(), attrs.end(), table_attr_parser());
    m_tables.push_back(parser.create_table());
    cout << "start table: " << m_tables.back().get_name() << endl;

    m_row = m_col = 0;
}

void ods_content_xml_context::end_table()
{
    cout << "end table" << endl;
}

void ods_content_xml_context::start_column(const xml_attrs_t& attrs)
{
}

void ods_content_xml_context::end_column()
{
}

void ods_content_xml_context::start_row(const xml_attrs_t& attrs)
{
    m_col = 0;
    m_row_attr = row_attr();
    for_each(attrs.begin(), attrs.end(), row_attr_parser(m_row_attr));
}

void ods_content_xml_context::end_row()
{
    if (m_row_attr.number_rows_repeated > 1)
    {
        // TODO: repeat this row.
        cout << "repeat this row " << m_row_attr.number_rows_repeated << " times" << endl;
    }
    m_row += m_row_attr.number_rows_repeated;
}

void ods_content_xml_context::start_cell(const xml_attrs_t& attrs)
{
    m_cell_attr = cell_attr();
    for_each(attrs.begin(), attrs.end(), cell_attr_parser(m_cell_attr));
}

void ods_content_xml_context::end_cell()
{
    cout << "cell: (row=" << m_row << "," << "col=" << m_col << ")" << endl;
    if (m_cell_attr.number_columns_repeated > 1)
    {
        // TODO: repeat this cell.
        cout << "repeat this cell " << m_cell_attr.number_columns_repeated << " times" << endl;
    }
    m_col += m_cell_attr.number_columns_repeated;
}

void ods_content_xml_context::print_html(const string& filepath) const
{
    ofstream file(filepath.c_str());
    file << "<html>" << endl;
    file << "<title>content.xml</title>" << endl;
    file << "<body>" << endl;
    for_each(m_tables.begin(), m_tables.end(), table_html_printer(file));
    file << "</body>" << endl;
    file << "</html>" << endl;
}

}
