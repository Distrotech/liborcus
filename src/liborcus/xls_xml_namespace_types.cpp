/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#include "xls_xml_namespace_types.hpp"

namespace orcus {

const xmlns_id_t NS_xls_xml_ss = "urn:schemas-microsoft-com:office:spreadsheet";
const xmlns_id_t NS_xls_xml_o ="urn:schemas-microsoft-com:office:office";
const xmlns_id_t NS_xls_xml_x ="urn:schemas-microsoft-com:office:excel";
const xmlns_id_t NS_xls_xml_html ="http://www.w3.org/TR/REC-html40";

namespace {

xmlns_id_t xls_xml_ns[] = {
    NS_xls_xml_ss,
    NS_xls_xml_o,
    NS_xls_xml_x,
    NS_xls_xml_html,
    NULL
};

}

const xmlns_id_t* NS_xls_xml_all = xls_xml_ns;

}
