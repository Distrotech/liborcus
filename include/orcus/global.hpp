/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_GLOBAL_HPP__
#define __ORCUS_GLOBAL_HPP__

#include "types.hpp"
#include "env.hpp"

#include <functional>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

#define ORCUS_ASCII(literal) literal, sizeof(literal)-1
#define ORCUS_N_ELEMENTS(name) sizeof(name)/sizeof(name[0])

namespace orcus {

class tokens;

void print_element(xmlns_id_t ns, xml_token_t name);

/**
 * Print attributes to stdout for debugging purposes.
 */
void print_attrs(const tokens& tokens, const xml_attrs_t& attrs);

/**
 * Parse the string representation of a date-time value, and convert it into
 * a set of numerical values.  A string representation allows either a date
 * only or a date and time value.  It does not allow a time only value; it
 * always expects to have a date element.
 *
 * date only: 2013-04-09
 * date and time: 2013-04-09T21:34:09.55
 *
 * @param str string representation of a date-time value.
 * @return converted date-time value consisting of a set of numeric values.
 */
date_time_t to_date_time(const pstring& str);

template<typename _T>
struct default_deleter : public std::unary_function<_T*, void>
{
    void operator() (_T* p)
    {
        delete p;
    }
};

/**
 * Function object for deleting objects that are stored in map container as
 * pointers.
 */
template<typename T>
struct map_object_deleter : public ::std::unary_function<typename T::value_type, void>
{
    void operator() (typename T::value_type& v)
    {
        delete v.second;
    }
};

template<typename _T, typename _Deleter = default_deleter<_T> >
class unique_ptr : public boost::interprocess::unique_ptr<_T, _Deleter>
{
public:
    unique_ptr() : boost::interprocess::unique_ptr<_T, _Deleter>(NULL) {}
    unique_ptr(_T* p) : boost::interprocess::unique_ptr<_T, _Deleter>(p) {}
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
