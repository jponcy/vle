/**
 * @file vle/gvle/OutputPlugin.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

#ifndef VLE_GVLE_OUTPUTPLUGIN_HPP
#define VLE_GVLE_OUTPUTPLUGIN_HPP

#include <gdkmm/pixbuf.h>
#include <vle/vpz/Output.hpp>
#include <vle/vpz/View.hpp>

namespace vle { namespace gvle {

/**
 * @brief A virtual class to define Output plug-ins.
 * @code
 * class TmpSelector : public vle::gvle::OutputPlugin
 * {
 * vle::vpz::Output& m_output;
 *
 * public:
 *   virtual void start(vpz::Output& output, vpz::View&)
 *   {
 *      output.location(Glib::get_tmp_dir());
 *   }
 * };
 * @endcode
 */
class OutputPlugin
{
public:
    OutputPlugin(const std::string& name);

    virtual ~OutputPlugin() {}

    /**
     * @brief Call to start the graphics interfaces. Use it to show the
     * Gtk::Dialog for instance. This function is called each time user select
     * the plug-in.
     * @param output The output to modify.
     * @param view The view to modify.
     * @return true if successful, otherwise false
     */
    virtual bool start(vpz::Output& output, vpz::View& view) = 0;

    /**
     * @brief Get a reference to the icon of this plug-in. If the plug-in have
     * not icon, or on error, the icon function return an empty pointer.
     * @return A reference to the icon or an empty pointer.
     */
    Glib::RefPtr < Gdk::Pixbuf > icon() const;

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;

    /**
     * @brief Name of the plug-in taken from library name.
     */
    std::string m_name;
};

}} // namespace vle gvle

#define DECLARE_GVLE_OUTPUTPLUGIN(x) \
    extern "C" { \
        vle::gvle::OutputPlugin* makeNewOutputPlugin( \
            const std::string& name) \
        { return new x(name); } \
    }

#endif
