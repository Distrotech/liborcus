/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/css_types.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

bool check_prop(const css_properties_t& props, const pstring& key, const pstring& val)
{
    css_properties_t::const_iterator it = props.find(key);
    if (it == props.end())
    {
        cout << "property '" << key << "' not found" << endl;
        return false;
    }

    if (it->second != val)
    {
        cout << "property '" << key << "' is expected to have value '"
            << val << "' but '" << it->second << "' is found." << endl;
        return false;
    }

    return true;
}

void test_css_parse_basic1()
{
    const char* path = SRCDIR"/test/css/basic1.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.name = "table";

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 2);
    assert(check_prop(*props, "width", "auto"));
    assert(check_prop(*props, "height", "500px"));

    selector.first.name = "td";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 2);
    assert(check_prop(*props, "color", "gray"));
    assert(check_prop(*props, "background-color", "yellow"));

    // This selector doesn't exist in the document tree.
    selector.first.name = "tr";
    props = doc.get_properties(selector);
    assert(!props);
}

int main()
{
    test_css_parse_basic1();
    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
