/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/yaml_parser.hpp"

namespace orcus {

class handler
{
};

yaml_document_tree::yaml_document_tree() {}
yaml_document_tree::~yaml_document_tree() {}

void yaml_document_tree::load(const std::string& strm)
{
    handler hdl;
    yaml_parser<handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
