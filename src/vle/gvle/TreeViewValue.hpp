/**
 * @file vle/gvle/TreeViewValue.hpp
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


#ifndef GUI_TREEVIEW_CONDITION_HPP
#define GUI_TREEVIEW_CONDITION_HPP

#include <gtkmm.h>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/XML.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>

namespace vle
{
namespace gvle {

class TreeViewValue : public Gtk::TreeView
{
public:
    //Tree model columns:
class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_name);
            add(m_col_type);
            add(m_col_view);
            add(m_col_value);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<Glib::ustring> m_col_type;
        Gtk::TreeModelColumn<Glib::ustring> m_col_view;
        Gtk::TreeModelColumn<value::Value> m_col_value;
    };

    TreeViewValue();
    virtual ~TreeViewValue();

    void makeTreeView(value::SetFactory& set);
    void makeTreeView(value::MapFactory& map);

    void refresh();

protected:
    virtual bool on_button_press_event(GdkEventButton* e);

private:
    ModelColumns m_Columns;
    //The Tree model:
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

    Gtk::Menu mMenu;

    value::ValueBase* mValue;

    void on_row_activated(const Gtk::TreeModel::Path& path,
                          Gtk::TreeViewColumn*  column);
    void on_menu_insert(value::ValueBase::type);
    void on_menu_remove();

};

}
} // namespace vle gvle
#endif