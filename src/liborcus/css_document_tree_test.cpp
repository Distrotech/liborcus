/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/css_types.hpp"
#include "orcus/stream.hpp"
#include "orcus/global.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

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

    // Chain all property values into a single string delimited by a " ".
    const vector<pstring>& vals = it->second;
    ostringstream os;
    if (vals.size() > 1)
    {
        vector<pstring>::const_iterator it_end = vals.end();
        advance(it_end, -1);
        copy(vals.begin(), it_end, ostream_iterator<pstring>(os, " "));
    }
    os << vals.back();

    string val_stored = os.str();
    if (val.str() != val_stored)
    {
        cout << "property '" << key << "' is expected to have value '"
            << val << "' but '" << val_stored << "' is found." << endl;
        return false;
    }

    return true;
}

void test_css_simple_selector_equality()
{
    css_simple_selector_t left;
    css_simple_selector_t right;
    assert(left == right);

    left.classes.insert("one");
    assert(left != right);
    right.classes.insert("one");
    assert(left == right);

    left.classes.insert("two");
    assert(left != right);
    right.classes.insert("two");
    assert(left == right);

    left.classes.insert("three");
    assert(left != right);
    right.classes.insert("three");
    assert(left == right);
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

void test_css_parse_basic2()
{
    const char* path = SRCDIR"/test/css/basic2.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.name = "div";
    selector.first.classes.insert("foo");

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "border", "solid 1px"));

    selector.clear();
    selector.first.classes.insert("warning");

    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 2);
    assert(check_prop(*props, "background-color", "red"));
    assert(check_prop(*props, "font-weight", "900"));
}

void test_css_parse_basic3()
{
    const char* path = SRCDIR"/test/css/basic3.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.name = "html";

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "height", "100%"));

    selector.first.name = "body";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "height", "100%"));

    {
        // h1, h2, h3 and h4 all have identical set of properties.
        const char* names[] = { "h1", "h2", "h3", "h4" };
        size_t n = ORCUS_N_ELEMENTS(names);

        for (size_t i = 0; i < n; ++i)
        {
            selector.first.name = names[i];
            props = doc.get_properties(selector);
            assert(props);
            assert(props->size() == 2);
            assert(check_prop(*props, "font-variant", "small-caps"));
            assert(check_prop(*props, "padding", "2.5em"));
        }
    }
}

void test_css_parse_basic4()
{
    const char* path = SRCDIR"/test/css/basic4.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.name = "h1";

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "margin", "0.5in"));

    selector.first.name = "h2";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "line-height", "3cm"));

    selector.first.name = "h3";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "word-spacing", "4mm"));

    selector.first.name = "h4";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "font-size", "1pc"));

    selector.first.name = "p";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "font-size", "12px"));
}

void test_css_parse_basic5()
{
    const char* path = SRCDIR"/test/css/basic5.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.classes.insert("info");

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "word-spacing", "normal"));
}

void test_css_parse_basic6()
{
    const char* path = SRCDIR"/test/css/basic6.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.name = "h1";
    selector.first.id = "chapter1";

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "text-align", "center"));

    selector.first.id = "z98y";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "letter-spacing", "0.5em"));

    selector.clear();
    selector.first.id = "id_global";
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "margin", "10px"));
}

void test_css_parse_basic7()
{
    const char* path = SRCDIR"/test/css/basic7.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
    selector.first.classes.insert("one");
    selector.first.classes.insert("two");
    selector.first.classes.insert("three");

    const css_properties_t* props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "color", "blue"));

    selector.first.classes.clear();
    selector.first.classes.insert("one");
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "color", "aqua"));

    selector.first.classes.clear();
    selector.first.classes.insert("two");
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "color", "azure"));

    selector.first.classes.insert("one"); // one two
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "color", "brown"));

    selector.first.clear();
    selector.first.name = "span";
    selector.first.classes.insert("button");
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "border", "solid 1px gray"));

    selector.first.classes.insert("selected"); // button selected
    props = doc.get_properties(selector);
    assert(props);
    assert(props->size() == 1);
    assert(check_prop(*props, "border", "solid 4px red"));
}

int main()
{
    test_css_simple_selector_equality();
    test_css_parse_basic1();
    test_css_parse_basic2();
    test_css_parse_basic3();
    test_css_parse_basic4();
    test_css_parse_basic5();
    test_css_parse_basic6();
    test_css_parse_basic7();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
