/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_GNUMERICCONTEXT_HPP__
#define __ORCUS_GNUMERICCONTEXT_HPP__

#include "xml_context_base.hpp"

#include <vector>
#include <boost/scoped_ptr.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

class gnumeric_content_xml_context : public xml_context_base
{
public:
    gnumeric_content_xml_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~gnumeric_content_xml_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:

private:
    spreadsheet::iface::import_factory* mp_factory;
    boost::scoped_ptr<xml_context_base> mp_child;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
