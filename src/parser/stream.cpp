/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/stream.hpp"
#include "orcus/exception.hpp"

#include <sstream>
#include <fstream>

using namespace std;

namespace orcus {

std::string load_file_content(const char* filepath)
{
    ifstream file(filepath);
    if (!file)
    {
        // failed to open the specified file.
        ostringstream os;
        os << "failed to load " << filepath;
        throw general_error(os.str());
    }

    ostringstream os;
    os << file.rdbuf();
    file.close();

    return os.str();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
