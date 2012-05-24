/*
 * @file vle/gvle/DynamicBox.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/gvle/DynamicBox.hpp>
#include <vle/gvle/OpenModelingPluginBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/NewDynamicsBox.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <fstream>
#include <cmath>

namespace vle { namespace gvle {

DynamicBox::DynamicBox(const Glib::RefPtr < Gtk::Builder >& xml,
                       GVLE* gvle,
                       vpz::AtomicModel& atom,
                       vpz::Dynamic& dynamic,
                       vpz::Conditions& conditions,
                       vpz::Observables& observables)
    : mXml(xml), mGVLE(gvle), mAtom(atom), mDynamic(dynamic),
    mConditions(conditions), mObservables(observables)
{
    xml->get_widget("DialogDynamic", mDialog);

    {
        xml->get_widget("HBoxDynamicLibrary", mBoxDynamicLibrary);
        mComboLibrary = Gtk::manage(new Gtk::ComboBoxText());
        mComboLibrary->show();

        Glib::RefPtr < Gtk::ListStore > toSortList =
            Glib::RefPtr < Gtk::ListStore >::cast_dynamic(mComboLibrary->get_model());
        toSortList->set_sort_column(0, Gtk::SORT_ASCENDING);

        mBoxDynamicLibrary->pack_start(*mComboLibrary);
    }

    {
        xml->get_widget("HBoxDynamicPackage", mBoxDynamicPackage);
        mComboPackage = Gtk::manage(new Gtk::ComboBoxText());
        mComboPackage->show();
        mBoxDynamicPackage->pack_start(*mComboPackage);
    }

    xml->get_widget("EntryDynamicLocationHost", mLocationHost);
    xml->get_widget("SpinbuttonDynamicLocationPort", mLocationPort);

    {
        xml->get_widget("HboxDynamicLanguage", mBoxDynamicLanguage);
        mLanguage = Gtk::manage(new Gtk::ComboBoxText());
        mBoxDynamicLanguage->pack_start(*mLanguage);
        mLanguage->show();
        mLanguage->append_text("c++");
        mLanguage->append_text("python");
    }

    xml->get_widget("buttonNewDynamicLibrary", mButtonNew);

    mList.push_back(mButtonNew->signal_clicked().connect(
            sigc::mem_fun(*this, &DynamicBox::onNewLibrary)));
    mList.push_back(mComboPackage->signal_changed().connect(
            sigc::mem_fun(*this, &DynamicBox::onComboPackageChange)));
}

DynamicBox::~DynamicBox()
{
    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }

    mBoxDynamicLibrary->remove(*mComboLibrary);
    mBoxDynamicPackage->remove(*mComboPackage);
    mBoxDynamicLanguage->remove(*mLanguage);
}

void DynamicBox::show(vpz::Dynamic* dyn)
{
    mDyn = dyn;

    mDialog->set_title((fmt(_("Dynamics: %1%")) % dyn->name()).str());
    makeComboPackage();
    makeComboLibrary();

    if (dyn->language().empty()) {
        mLanguage->set_active_text("c++");
    } else {
        mLanguage->set_active_text(dyn->language());
    }

    if (mDialog->run() == Gtk::RESPONSE_OK) {
        mDialog->hide();
        on_apply();
    } else {
        mDialog->hide();
        on_cancel();
    }
}

void DynamicBox::makeComboLibrary()
{
    mComboLibrary->clear();

    utils::ModuleList lst;
    utils::ModuleList::iterator it;

    std::string selectedPackage = mComboPackage->get_active_text();
    mGVLE->pluginFactory().getDynamicsPlugins(selectedPackage, &lst);

    for (it = lst.begin(); it != lst.end(); ++it) {
        mComboLibrary->append_text(it->library);
    }

    mComboLibrary->set_active_text(mDyn->library());
}

void DynamicBox::makeComboPackage()
{
    mComboPackage->clear();

    utils::PathList paths = utils::Path::path().getInstalledPackages();

    std::sort(paths.begin(), paths.end());
    for (utils::PathList::const_iterator i = paths.begin(), e = paths.end();
         i != e; ++i) {
        mComboPackage->append_text(*i);
    }

    mComboPackage->set_active_text(mDyn->package());
}

void DynamicBox::onComboPackageChange()
{
    makeComboLibrary();
}

void DynamicBox::on_apply()
{
    mValid = true;
    if (mComboLibrary->get_active_text().empty()) {
        Error(_("Set a library to this Dynamic"));
        mValid = false;
        return;
    }

    mDyn->setLibrary(mComboLibrary->get_active_text());
    mDyn->setPackage(mComboPackage->get_active_text());

    if (mLanguage->get_active_text() == "c++") {
        mDyn->setLanguage("");
    } else {
        mDyn->setLanguage(mLanguage->get_active_text());
    }
}

void DynamicBox::on_cancel()
{
    mValid = false;
}

void DynamicBox::onNewLibrary()
{
    OpenModelingPluginBox box(mXml, mGVLE);

    if (box.run() == Gtk::RESPONSE_OK) {
        NewDynamicsBox box2(mXml);

        if (box2.run() == Gtk::RESPONSE_OK and
            not box2.getClassName().empty() and
            not box2.getNamespace().empty()) {
            if (execPlugin(box.pluginName(),
                           box2.getClassName(),
                           box2.getNamespace()) == Gtk::RESPONSE_OK) {
                mComboPackage->set_active_text(box2.getNamespace());
                mComboLibrary->append_text(box2.getClassName());
                mComboLibrary->set_active_text(box2.getClassName());
            }
        }
    }
}

int DynamicBox::execPlugin(const std::string& pluginname,
                           const std::string& classname,
                           const std::string& namespace_)
{
    ModelingPluginPtr plugin =
        mGVLE->pluginFactory().getModelingPlugin(pluginname);

    if (plugin->create(mAtom, mDynamic, mConditions,
                       mObservables, classname, namespace_)) {
        const std::string& buffer = plugin->source();
        std::string filename = utils::Path::path().getPackageSrcFile(classname);
        filename += ".cpp";

        try {
            std::ofstream f(filename.c_str());
            f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            f << buffer;
        } catch(const std::ios_base::failure& e) {
            throw utils::ArgError(fmt(
                    _("Cannot store buffer in file '%1%'")) % filename);
        }

        return Gtk::RESPONSE_OK;
    } else {
        return Gtk::RESPONSE_CANCEL;
    }
}

}} // namespace vle gvle
