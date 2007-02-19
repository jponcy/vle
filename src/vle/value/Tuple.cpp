/**
 * @file value/Tuple.cpp
 * @author The VLE Development Team.
 * @brief Tuple Value a container to Value class.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/value/Tuple.hpp>

namespace vle { namespace value {

TupleFactory::TupleFactory(const TupleFactory& setfactory) :
    ValueBase(setfactory),
    m_value(setfactory.m_value)
{ }

Tuple TupleFactory::create()
{
    return Tuple(new TupleFactory());
}

Value TupleFactory::clone() const
{
    return Value(new TupleFactory(*this));
}

std::string TupleFactory::toFile() const
{
    std::string s;

    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
	s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s+= " ";
    }
    return s;
}

std::string TupleFactory::toString() const
{
    std::string s = "(";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
	s += boost::lexical_cast < std::string >(*it);
	if (it + 1 != m_value.end())
	    s += ",";
    }
    s += ")";
    return s;
}

std::string TupleFactory::toXML() const
{
    std::string s = "<tuple>";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s += " ";
    }
    s += "</tuple>";
    return s;
}

}} // namespace vle value
