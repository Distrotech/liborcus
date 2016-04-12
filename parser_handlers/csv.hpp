/*************************************************************************
 *
 * Copyright (c) 2015 Kohei Yoshida
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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_CSV_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_CSV_HPP

#include <cstdlib>

class csv_parser_handler
{
public:
    /**
     * Called when the parser starts parsing a stream.
     */
    void begin_parse() {}

    /**
     * Called when the parser finishes parsing a stream.
     */
    void end_parse() {}

    /**
     * Called at the beginning of every row.
     */
    void begin_row() {}

    /**
     * Called at the end of every row.
     */
    void end_row() {}

    /**
     * Called after every cell is parsed.
     *
     * @param p pointer to the first character of a cell content.
     * @param n number of characters the cell content consists of.
     */
    void cell(const char* p, size_t n) {}
};

#endif
