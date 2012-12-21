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

#include "orcus/xml_namespace.hpp"
#include "orcus/pstring.hpp"

#include <cstdlib>
#include <cassert>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace orcus;

namespace {

void test_basic()
{
    pstring xmlns1("http://some.xmlns/");
    pstring xmlns2("http://other.xmlns/");

    xmlns_repository repo;
    xmlns_context cxt1 = repo.create_context();
    xmlns_context cxt2 = repo.create_context();

    pstring empty, myns("myns");
    {
        // context 1
        xmlns_id_t test1 = cxt1.push(empty, xmlns1); // register default namespace.
        assert(cxt1.get(empty) == test1);
        xmlns_id_t test2 = cxt1.push(myns, xmlns2);
        assert(cxt1.get(myns) == test2);
        assert(test1 != test2);
    }

    {
        // context 2
        xmlns_id_t test1 = cxt2.push(empty, xmlns2); // register default namespace.
        assert(cxt2.get(empty) == test1);
        xmlns_id_t test2 = cxt2.push(myns, xmlns1);
        assert(cxt2.get(myns) == test2);
        assert(test1 != test2);
    }

    // Now, compare the registered namespaces between the two namespaces.
    assert(cxt1.get(empty) == cxt2.get(myns));
    assert(cxt1.get(myns) == cxt2.get(empty));
}

void test_all_namespaces()
{
    pstring key1("a"), key2("b"), key3("c");
    pstring ns1("foo"), ns2("baa"), ns3("hmm");

    xmlns_repository repo;
    xmlns_context cxt = repo.create_context();
    xmlns_id_t ns;

    ns = cxt.push(key1, ns1);
    assert(ns1 == ns);
    ns = cxt.push(key2, ns2);
    assert(ns2 == ns);
    ns = cxt.push(key3, ns3);
    assert(ns3 == ns);

    vector<xmlns_id_t> all_ns;
    cxt.get_all_namespaces(all_ns);
    assert(all_ns.size() == 3);
    assert(ns1 == all_ns[0]);
    assert(ns2 == all_ns[1]);
    assert(ns3 == all_ns[2]);
}

const xmlns_id_t NS_test_name1 = "test:name:1";
const xmlns_id_t NS_test_name2 = "test:name:2";
const xmlns_id_t NS_test_name3 = "test:name:3";

xmlns_id_t NS_test_all[] = {
    NS_test_name1,
    NS_test_name2,
    NS_test_name3,
    NULL
};

void test_predefined_ns()
{
    xmlns_repository ns_repo;
    ns_repo.add_predefined_values(NS_test_all);
    xmlns_context cxt = ns_repo.create_context();
    xmlns_id_t ns_id = cxt.push("tn1", "test:name:1");
    assert(ns_id == NS_test_name1);
    ns_id = cxt.push("tn2", "test:name:2");
    assert(ns_id == NS_test_name2);
    ns_id = cxt.push("tn3", "test:name:3");
    assert(ns_id == NS_test_name3);
    assert(cxt.get("tn1") == NS_test_name1);
    assert(cxt.get("tn2") == NS_test_name2);
    assert(cxt.get("tn3") == NS_test_name3);
}

} // anonymous namespace

int main()
{
    test_basic();
    test_all_namespaces();
    test_predefined_ns();
    return EXIT_SUCCESS;
}
