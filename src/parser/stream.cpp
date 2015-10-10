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
#include <tuple>
#include <cassert>

namespace orcus {

namespace {

std::tuple<std::string, size_t, size_t> find_line_with_offset(
    const std::string& strm, std::ptrdiff_t offset)
{
    const char* p0 = strm.data();
    const char* p_end = p0 + strm.size();
    const char* p_offset = p0 + offset;

    // Determine the line number.
    size_t line_num = 1;
    for (const char* p = p0; p != p_offset; ++p)
    {
        if (*p == '\n')
            ++line_num;
    }

    // Determine the beginning of the line.
    const char* p_line_start = p_offset;
    for (; p0 <= p_line_start; --p_line_start)
    {
        if (*p_line_start == '\n')
        {
            ++p_line_start;  // 1st char of the line.
            break;
        }
    }

    // Determine the end of the line.
    const char* p_line_end = p_offset;
    for (; p_line_end < p_end; ++p_line_end)
    {
        if (*p_line_end == '\n')
            // one character after the last character of the line.
            break;
    }

    assert(p_line_start <= p_offset);
    size_t offset_on_line = std::distance(p_line_start, p_offset);
    std::string line(p_line_start, p_line_end - p_line_start);

    return std::make_tuple(line, line_num, offset_on_line);
}

}

std::string load_file_content(const char* filepath)
{
    std::ifstream file(filepath);
    if (!file)
    {
        // failed to open the specified file.
        std::ostringstream os;
        os << "failed to load " << filepath;
        throw general_error(os.str());
    }

    std::ostringstream os;
    os << file.rdbuf();
    file.close();

    return os.str();
}

std::string create_parse_error_output(const std::string& strm, std::ptrdiff_t offset)
{
    if (offset < 0)
        return std::string();

    auto line_info = find_line_with_offset(strm, offset);
    std::string line = std::get<0>(line_info);
    size_t line_num = std::get<1>(line_info);
    size_t offset_on_line = std::get<2>(line_info);
    std::ostringstream os;
    os << line_num << ": ";
    size_t line_num_width = os.str().size();

    os << line << std::endl;

    for (size_t i = 0; i < (offset_on_line+line_num_width); ++i)
        os << ' ';
    os << '^' << std::endl;

    return os.str();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
