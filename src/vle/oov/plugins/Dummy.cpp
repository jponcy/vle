/*
 * @file vle/oov/plugins/Dummy.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/oov/plugins/Dummy.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>


namespace vle { namespace oov { namespace plugin {

Dummy::Dummy(const std::string& location) :
    Plugin(location)
{
}

Dummy::~Dummy()
{
}

void Dummy::onParameter(const std::string& /*plugin*/,
                         const std::string& /*location*/,
                         const std::string& /*file*/,
                         value::Value* parameters,
                         const double& /*time*/)
{
    delete parameters;
}

void Dummy::onNewObservable(const std::string& /*simulator*/,
                             const std::string& /*parent*/,
                             const std::string& /*port*/,
                             const std::string& /*view*/,
                             const double& /*time*/)
{
}

void Dummy::onDelObservable(const std::string& /*simulator*/,
                             const std::string& /*parent*/,
                             const std::string& /*port*/,
                             const std::string& /*view*/,
                             const double& /*time*/)
{
}

void Dummy::onValue(const std::string& /*simulator*/,
                     const std::string& /*parent*/,
                     const std::string& /*port*/,
                     const std::string& /*view*/,
                     const double& /*time*/,
                     value::Value* value)
{
    delete value;
}

void Dummy::close(const double& /*time*/)
{
}

}}} // namespace vle oov plugin