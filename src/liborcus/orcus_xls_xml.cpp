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

#include "orcus/orcus_xls_xml.hpp"
#include "orcus/stream.hpp"
#include "orcus/xml_namespace.hpp"

#include "xml_stream_parser.hpp"
#include "xls_xml_handler.hpp"
#include "xls_xml_detection_handler.hpp"
#include "session_context.hpp"
#include "xls_xml_tokens.hpp"
#include "xls_xml_namespace_types.hpp"
#include "detection_result.hpp"

#include <boost/scoped_ptr.hpp>

#define ORCUS_DEBUG_XLS_XML_FILTER 1

#if ORCUS_DEBUG_XLS_XML_FILTER
#include <iostream>
#endif

using namespace std;

namespace orcus {

struct orcus_xls_xml_impl
{
    xmlns_repository m_ns_repo;
    session_context m_cxt;
    spreadsheet::iface::import_factory* mp_factory;

    orcus_xls_xml_impl(spreadsheet::iface::import_factory* factory) : mp_factory(factory) {}
};

orcus_xls_xml::orcus_xls_xml(spreadsheet::iface::import_factory* factory) :
    mp_impl(new orcus_xls_xml_impl(factory))
{
    mp_impl->m_ns_repo.add_predefined_values(NS_xls_xml_all);
}

orcus_xls_xml::~orcus_xls_xml()
{
    delete mp_impl;
}

bool orcus_xls_xml::detect(const unsigned char* buffer, size_t size)
{
    xmlns_repository ns_repo;
    ns_repo.add_predefined_values(NS_xls_xml_all);
    xml_stream_parser parser(ns_repo, xls_xml_tokens, reinterpret_cast<const char*>(buffer), size, "content");

    session_context cxt;
    xls_xml_detection_handler handler(cxt, xls_xml_tokens);
    parser.set_handler(&handler);
    try
    {
        parser.parse();
    }
    catch (const detection_result& res)
    {
        return res.get_result();
    }
    catch (...) {}

    return false;
}

void orcus_xls_xml::read_file(const char* fpath)
{
#if ORCUS_DEBUG_XLS_XML_FILTER
    cout << "reading " << fpath << endl;
#endif

    string strm;
    load_file_content(fpath, strm);
    if (strm.empty())
        return;

    xml_stream_parser parser(mp_impl->m_ns_repo, xls_xml_tokens, &strm[0], strm.size(), "content");
    boost::scoped_ptr<xls_xml_handler> handler(
        new xls_xml_handler(mp_impl->m_cxt, xls_xml_tokens, mp_impl->mp_factory));
    parser.set_handler(handler.get());
    parser.parse();
}

}
