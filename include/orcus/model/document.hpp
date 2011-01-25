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

#ifndef __ORCUS_MODEL_DOCUMENT_HPP__
#define __ORCUS_MODEL_DOCUMENT_HPP__

#include "orcus/model/interface.hpp"
#include "orcus/model/sheet.hpp"
#include "orcus/pstring.hpp"

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>

namespace orcus { namespace model {

class shared_strings;

/**
 * Internal document representation used only for testing the filters.
 */
class document : private ::boost::noncopyable
{
    /**
     * Single sheet entry which consists of a sheet name and a sheet data. 
     * Use the printer function object to print sheet content with for_each 
     * function. 
     */
    struct sheet_item : private ::boost::noncopyable
    {
        pstring name;
        sheet   data;
        sheet_item(const pstring& _name);

        struct printer : public ::std::unary_function<void, sheet_item>
        {
            void operator() (const sheet_item& item) const;
        };
    };

public:
    document();
    ~document();

    shared_strings* get_shared_strings();
    sheet* append_sheet(const pstring& sheet_name);

    /**
     * Dump document content to stdout for debugging.
     */
    void dump() const;

private:
    ::boost::ptr_vector<sheet_item> m_sheets;
    shared_strings* mp_strings;
};

}}

#endif
