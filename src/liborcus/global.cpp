/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#include "orcus/global.hpp"
#include "orcus/tokens.hpp"
#include "orcus/exception.hpp"

#include <iostream>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

struct attr_printer : unary_function<void, xml_token_attr_t>
{
public:
    attr_printer(const tokens& tokens) :
        m_tokens(tokens) {}

    void operator()(const xml_token_attr_t& attr) const
    {
        cout << "  ";
        if (attr.ns != XMLNS_UNKNOWN_ID)
            cout << attr.ns << ":";

        cout << m_tokens.get_token_name(attr.name) << " = \"" << attr.value << "\"" << endl;
    }

private:
    const tokens& m_tokens;
};

}

void print_element(const tokens& tokens, xmlns_id_t ns, xml_token_t name)
{
    if (ns != XMLNS_UNKNOWN_ID)
        cout << ns << ":";
    cout << tokens.get_token_name(name) << endl;
}

void print_attrs(const tokens& tokens, const xml_attrs_t& attrs)
{
    for_each(attrs.begin(), attrs.end(), attr_printer(tokens));
}

namespace {

void process_char(const char* p, const char*& digit, size_t& digit_len)
{
    if (!digit)
    {
        digit = p;
        digit_len = 1;
        return;
    }

    ++digit_len;
}

void flush_int(int& store, const char*& digit, size_t& digit_len)
{
    store = strtol(digit, NULL, 10);
    digit = NULL;
    digit_len = 0;
}

}

date_time_t to_date_time(const pstring& str)
{
    date_time_t ret;
    int dash_count = 0, t_count = 0, colon_count = 0;

    const char* p = str.get();
    const char* p_end = p + str.size();
    const char* digit = p;
    size_t digit_len = 0;

    bool valid = true;
    for (; p != p_end && valid; ++p)
    {
        switch (*p)
        {
            case '-':
            {
                if (t_count || colon_count || !digit)
                {
                    // Invalid date-time value.  All dashes must occur before
                    // any of 'T' and ':' occur.
                    valid = false;
                    break;
                }

                switch (dash_count)
                {
                    case 0:
                        // Flush year.
                        flush_int(ret.year, digit, digit_len);
                    break;
                    case 1:
                        // Flush month.
                        flush_int(ret.month, digit, digit_len);
                    break;
                    default:
                        valid = false;
                }
                ++dash_count;
            }
            break;
            case 'T':
            {
                if (t_count || dash_count != 2 || !digit)
                {
                    // Invalid date-time value.
                    valid = false;
                    break;
                }

                // Flush day.
                ret.day = strtol(digit, NULL, 10);
                digit = NULL;
                digit_len = 0;

                ++t_count;
            }
            break;
            case ':':
            {
                if (!t_count || !digit)
                {
                    // Invalid date-time value.
                    valid = false;
                    break;
                }

                switch (colon_count)
                {
                    case 0:
                        // Flush hour.
                        flush_int(ret.hour, digit, digit_len);
                    break;
                    case 1:
                        // Flush minute.
                        flush_int(ret.minute, digit, digit_len);
                    break;
                    default:
                        valid = false;
                }

                ++colon_count;
            }
            break;
            default:
            {
                if (t_count)
                {
                    // Time element.
                    switch (colon_count)
                    {
                        case 0:
                            // Hour
                            process_char(p, digit, digit_len);
                        break;
                        case 1:
                            // Minute
                            process_char(p, digit, digit_len);
                        break;
                        case 2:
                            // Second
                            process_char(p, digit, digit_len);
                        break;
                        default:
                            valid = false;
                    }
                }
                else
                {
                    // Date element.
                    switch (dash_count)
                    {
                        case 0:
                            // Year
                            process_char(p, digit, digit_len);
                        break;
                        case 1:
                            // Month
                            process_char(p, digit, digit_len);
                        break;
                        case 2:
                            // Day
                            process_char(p, digit, digit_len);
                        break;
                        default:
                            valid = false;
                    }
                }
            }
        }

    }

    if (!valid || !digit)
        return ret;

    if (t_count)
    {
        // Flush second.
        ret.second = strtod(digit, NULL);
    }
    else
    {
        // Flush day.
        ret.day = strtol(digit, NULL, 10);
    }

    return ret;
}

}
