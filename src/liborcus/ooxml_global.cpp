/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_global.hpp"
#include "ooxml_types.hpp"
#include "ooxml_token_constants.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace orcus {

void print_opc_rel::operator() (const opc_rel_t& v) const
{
    using namespace std;
    cout << v.rid << ": " << v.target << " (" << v.type << ")" << endl;
}

std::string resolve_file_path(const std::string& dir_path, const std::string& file_name)
{
    if (dir_path.empty())
        return file_name;

    const char* p = &dir_path[0];
    const char* p_end = p + dir_path.size();

    bool has_root = *p == '/';
    if (has_root)
        ++p;

    vector<pstring> dir_stack;
    const char* p_head = NULL;
    for (; p != p_end; ++p)
    {
        if (*p == '/')
        {
            if (!p_head)
                // invalid directory path.
                return file_name;

            size_t len = p - p_head;
            pstring dir(p_head, len);
            if (dir == "..")
            {
                if (dir_stack.empty())
                    // invalid directory path.
                    return file_name;

                dir_stack.pop_back();
            }
            else
                dir_stack.push_back(dir);

            p_head = NULL;
        }
        else if (p_head)
        {
            // Do nothing.
        }
        else
            p_head = p;
    }

    if (p_head)
    {
        // directory path must end with '/'.  This one doesn't.
        return file_name;
    }

    ostringstream full_path;
    if (has_root)
        full_path << '/';

    vector<pstring>::const_iterator it = dir_stack.begin(), it_end = dir_stack.end();
    for (; it != it_end; ++it)
        full_path << *it << '/';

    full_path << file_name;

    return full_path.str();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
