/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include "ooxml_namespace_types.hpp"

namespace orcus {

const xmlns_id_t NS_ooxml_r    = "http://schemas.openxmlformats.org/officeDocument/2006/relationships";
const xmlns_id_t NS_ooxml_xlsx = "http://schemas.openxmlformats.org/spreadsheetml/2006/main";

const xmlns_id_t NS_opc_ct  = "http://schemas.openxmlformats.org/package/2006/content-types";
const xmlns_id_t NS_opc_rel = "http://schemas.openxmlformats.org/package/2006/relationships";

namespace {

xmlns_id_t ooxml_ns[] = {
    NS_ooxml_r,
    NS_ooxml_xlsx,
    NULL
};

xmlns_id_t opc_ns[] = {
    NS_opc_ct,
    NS_opc_rel,
    NULL
};

}

const xmlns_id_t* NS_ooxml_all = ooxml_ns;
const xmlns_id_t* NS_opc_all = opc_ns;


}
