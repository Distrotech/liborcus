/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CONFIG_HPP
#define INCLUDED_ORCUS_CONFIG_HPP

#include "env.hpp"

#include <string>

namespace orcus {

struct ORCUS_DLLPUBLIC config
{
    /**
     * Enable or disable runtime debug output to stdout or stderr.
     */
    bool debug;

    /**
     * Control whether or not to perform strict check of the xml structure of
     * a stream being parsed.  When enabled, it throws an xml_structure_error
     * exception when an incorrect xml structure is detected.
     */
    bool structure_check;

    config();
};

struct ORCUS_DLLPUBLIC json_config
{
    enum class output_format_type { none, xml, json, check };

    std::string input_path;
    std::string output_path;
    output_format_type output_format;

    /**
     * Control whether or not to preserve the order of object's child
     * name/value pairs.  By definition, JSON's object is an unordered set of
     * name/value pairs, but in some cases preserving the original order may
     * be desirable.
     */
    bool preserve_object_order;

    /**
     * Control whether or not to resolve JSON references to external files.
     */
    bool resolve_references;

    /**
     * When true, the document tree should allocate memory and hold copies of
     * string values in the tree.  When false, no extra memory is allocated
     * for string values in the tree and the string values simply point to the
     * original json string stream.
     *
     * In other words, when this option is set to false, the caller must
     * ensure that the json string stream instance stays alive for the entire
     * life cycle of the document tree.
     */
    bool persistent_string_values;

    json_config();
    ~json_config();
};

struct ORCUS_DLLPUBLIC yaml_config
{
    enum class output_format_type { none, yaml, json };

    std::string input_path;
    std::string output_path;

    output_format_type output_format;

    yaml_config();
    ~yaml_config();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
