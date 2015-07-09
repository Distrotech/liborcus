/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_selector.hpp"

#include <cstring>

namespace orcus {

css_simple_selector_t::css_simple_selector_t() :
    pseudo_classes(0) {}

void css_simple_selector_t::clear()
{
    name.clear();
    id.clear();
    classes.clear();
    pseudo_classes = 0;
}

bool css_simple_selector_t::empty() const
{
    return name.empty() && id.empty() && classes.empty() && !pseudo_classes;
}

bool css_simple_selector_t::operator== (const css_simple_selector_t& r) const
{
    if (name != r.name)
        return false;

    if (id != r.id)
        return false;

    if (classes != r.classes)
        return false;

    return pseudo_classes == r.pseudo_classes;
}

bool css_simple_selector_t::operator!= (const css_simple_selector_t& r) const
{
    return !operator==(r);
}

size_t css_simple_selector_t::hash::operator() (const css_simple_selector_t& ss) const
{
    static pstring::hash hasher;

    size_t val = hasher(ss.name);
    val += hasher(ss.id);
    classes_type::const_iterator it = ss.classes.begin(), ite = ss.classes.end();
    for (; it != ite; ++it)
        val += hasher(*it);

    val += ss.pseudo_classes;

    return val;
}

bool css_chained_simple_selector_t::operator== (const css_chained_simple_selector_t& r) const
{
    return combinator == r.combinator && simple_selector == r.simple_selector;
}

css_chained_simple_selector_t::css_chained_simple_selector_t() :
    combinator(css::combinator_t::descendant) {}

css_chained_simple_selector_t::css_chained_simple_selector_t(const css_simple_selector_t& ss) :
    combinator(css::combinator_t::descendant), simple_selector(ss) {}

css_chained_simple_selector_t::css_chained_simple_selector_t(
    css::combinator_t op, const css_simple_selector_t& ss) :
    combinator(op), simple_selector(ss) {}

void css_selector_t::clear()
{
    first.clear();
    chained.clear();
}

bool css_selector_t::operator== (const css_selector_t& r) const
{
    return first == r.first && chained == r.chained;
}

css_property_value_t::css_property_value_t() :
    type(css::property_value_t::none), str(NULL) {}

css_property_value_t::css_property_value_t(const css_property_value_t& r) :
    type(r.type)
{
    switch (type)
    {
        case css::property_value_t::rgb:
        case css::property_value_t::rgba:
            red = r.red;
            green = r.green;
            blue = r.blue;
            alpha = r.alpha;
        break;
        case css::property_value_t::hsl:
        case css::property_value_t::hsla:
            hue = r.hue;
            saturation = r.saturation;
            lightness = r.lightness;
            alpha = r.alpha;
        break;
        case css::property_value_t::string:
        case css::property_value_t::url:
            str = r.str;
            length = r.length;
        break;
        case css::property_value_t::none:
        default:
            ;
    }
}

css_property_value_t::css_property_value_t(const pstring& str) :
    type(css::property_value_t::string), str(str.get()), length(str.size()) {}

css_property_value_t& css_property_value_t::operator= (const css_property_value_t& r)
{
    if (&r != this)
        std::memcpy(this, &r, sizeof(css_property_value_t));
    return *this;
}

void css_property_value_t::swap(css_property_value_t& r)
{
    unsigned char buf[sizeof(css_property_value_t)];
    std::memcpy(buf, this, sizeof(css_property_value_t));
    std::memcpy(this, &r, sizeof(css_property_value_t));
    std::memcpy(&r, buf, sizeof(css_property_value_t));
}

std::ostream& operator<< (std::ostream& os, const css_simple_selector_t& v)
{
    os << v.name;
    css_simple_selector_t::classes_type::const_iterator it = v.classes.begin(), ite = v.classes.end();
    for (; it != ite; ++it)
        os << '.' << *it;
    if (!v.id.empty())
        os << '#' << v.id;
    if (v.pseudo_classes)
        os << css::pseudo_class_to_string(v.pseudo_classes);
    return os;
}

std::ostream& operator<< (std::ostream& os, const css_selector_t& v)
{
    os << v.first;
    css_selector_t::chained_type::const_iterator it = v.chained.begin(), ite = v.chained.end();
    for (; it != ite; ++it)
    {
        const css_chained_simple_selector_t& css = *it;
        os << ' ';
        switch (css.combinator)
        {
            case css::combinator_t::direct_child:
                os << "> ";
            break;
            case css::combinator_t::next_sibling:
                os << "+ ";
            break;
            case css::combinator_t::descendant:
            default:
                ;
        }
        os << css.simple_selector;
    }
    return os;
}

std::ostream& operator<< (std::ostream& os, const css_property_value_t& v)
{
    const char* sep = ",";

    switch (v.type)
    {
        case css::property_value_t::hsl:
            os << "hsl("
               << (int)v.hue << sep
               << (int)v.saturation << sep
               << (int)v.lightness
               << ")";
        break;
        case css::property_value_t::hsla:
            os << "hsla("
               << (int)v.hue << sep
               << (int)v.saturation << sep
               << (int)v.lightness << sep
               << v.alpha
               << ")";
        break;
        case css::property_value_t::rgb:
            os << "rgb("
               << (int)v.red << sep
               << (int)v.green << sep
               << (int)v.blue
               << ")";
        break;
        case css::property_value_t::rgba:
            os << "rgba("
               << (int)v.red << sep
               << (int)v.green << sep
               << (int)v.blue << sep
               << v.alpha
               << ")";
        break;
        case css::property_value_t::string:
            os << pstring(v.str, v.length);
        break;
        case css::property_value_t::url:
            os << "url(" << pstring(v.str, v.length) << ")";
        break;
        break;
        case css::property_value_t::none:
        default:
            ;
    }

    return os;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
