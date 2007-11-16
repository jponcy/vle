/** 
 * @file extension/QSS.cpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 27 oct 2006 00:06:52 +0200
 */

/*
 * Copyright (C) 2006, 07 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/extension/QSS.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Debug.hpp>
#include <cmath>

namespace vle { namespace extension {

using namespace devs;
using namespace graph;
using namespace value;

qss::qss(const AtomicModel& model) :
    Dynamics(model),
    mActive(true),
    mDependance(true)
{
}

void qss::processInitEvents(const InitEventList& event)
{
    const value::Value& precision = event.get("precision");
    mPrecision = value::toDouble(precision);
    mEpsilon = mPrecision;

    const value::Value& threshold = event.get("threshold");
    mThreshold = value::toDouble(threshold);

    if (event.exist("active")) {
	const value::Value& active = event.get("active");
	mActive = value::toBoolean(active);
    }

    if (event.exist("dependance")) {
	const value::Value& dependance = event.get("dependance");
	mDependance = value::toBoolean(dependance);
    }

    const value::Value& value = event.get("value");
    mInitialValue = value::toDouble(value);

    const value::Value& name = event.get("name");
    mVariableName = value::toString(name);
}

void qss::updateSigma()
{
    if (std::abs(mGradient) < mThreshold) {
        mSigma = devs::Time::infinity;
    } else {
        if (mGradient > 0) {
            mSigma = devs::Time((d(mIndex + 1)-mValue) /
				mGradient);
	} else {
	    mSigma = devs::Time(((d(mIndex)-mValue)-mEpsilon) /
				mGradient);
	}
    }
}

void qss::reset(const Time& time, double value)
{
    mValue = value;
    mIndex = (long)(floor(mValue/mPrecision));
    mLastTime = time;
    mGradient = compute();
    updateSigma();    
}

// DEVS Methods

void qss::finish()
{
}

Time qss::init()
{
    mValue = mInitialValue;
    mGradient = 0.0;
    mIndex = (long)(floor(mValue/mPrecision));
    mSigma = devs::Time(0);
    mLastTime = devs::Time(0);
    mState = INIT;

    return devs::Time(0);
}


void qss::getOutputFunction(const Time& time,
			    ExternalEventList& output) 
{
    if (mState == INIT or ((mState == POST3 and mExternalValues) or (mState == RUN and mActive))) {
//    if (mState == INIT or (mState == RUN and mActive)) {
	devs::ExternalEvent* ee = new devs::ExternalEvent("update");
	double e = (time - mLastTime).getValue();
	
	ee << devs::attribute("name", mVariableName);
	ee << devs::attribute("value", mValue+e*mGradient);
	ee << devs::attribute("gradient", mGradient);
	output.addEvent(ee);
    }
}

Time qss::getTimeAdvance()
{
    return mSigma;
}

Event::EventType qss::processConflict(const InternalEvent& /* internal */,
				      const ExternalEventList& /* extEventlist */) const
{
    return Event::EXTERNAL;
}

void qss::processInternalEvent(const InternalEvent& event)
{
    switch (mState) {
    case INIT: // init du gradient
	if (mDependance) {
	    mState = POST_INIT;
	    mSigma = Time::infinity;
	}
	else {
	    mState = RUN;
	    mGradient = compute();
	    updateSigma();
	}
        break;
    case POST2:
	// mise � jour du gradient apr�s reception des valeurs de
	// mes variables externes
	mState = RUN;
	mLastTime = event.getTime();
	mGradient = compute();
	updateSigma();
        break;
    case POST3:
	// mise � jour du gradient apr�s reception d'une nouvelle
	// valeur pour l'une de mes variables externes
	// et envoie de ma nouvelle valeur aux �quations qui
	// d�pendent de moi si elles existent
	mState = RUN;
	updateSigma();
        break;
    case RUN:
        if (mGradient >= 0) ++mIndex;
        else --mIndex;
	mValue = d(mIndex);
	// Propagation ou non de la nouvelle valeur
        if (mActive and mExternalValues) {
	    // si oui alors on va attendre les valeurs
	    // actualis�es de toutes mes variables externes
	    mState = POST;
	    mSigma = Time::infinity; 
        } else {
	    // sinon on passe au pas suivant
	    mState = RUN;
	    mLastTime = event.getTime();
	    mGradient = compute();
	    updateSigma();
	}
	break;
    case POST:
    case POST_INIT:
	break;
    }
}

void qss::processExternalEvents(const ExternalEventList& event,
				const Time& time)
{
    if (mState == POST_INIT) // r�ception de la d�finition des variables externes
    {
	ExternalEventList::const_iterator it = event.begin();
	unsigned int index = 0;
	unsigned int linear = 0;

	while (it != event.end()) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");

	    mExternalVariableIndex[name] = index;
	    mExternalVariableValue[index] = value;
	    if (mIsGradient[index] = (*it)->existAttributeValue("gradient")) {
		mExternalVariableGradient[index] = (*it)->getDoubleAttributeValue("gradient");
		++linear;
	    }
	    ++index;
	    ++it;
	}
	mExternalValues = (linear < mExternalVariableIndex.size());
	mExternalVariableNumber = linear;
	mState = RUN;
	mGradient = compute();
	updateSigma();
    }
    else {
	ExternalEventList::const_iterator it = event.begin();
	bool _reset = false;
	
	while (it != event.end()) {
	    std::string name = (*it)->getStringAttributeValue("name");
	    double value = (*it)->getDoubleAttributeValue("value");
	    
	    // c'est une variable externe num�rique
	    if ((*it)->onPort("update")) {
		Assert(utils::InternalError, name != mVariableName,
		       boost::format("Qss update, invalid variable name: %1%") % name);
		
		setValue(name, value);
		if (mIsGradient[mExternalVariableIndex[name]])
		    setGradient(name, (*it)->getDoubleAttributeValue("gradient"));
	    }	    
	    // c'est une perturbation sur une variable interne
	    if ((*it)->onPort("perturb")) {
		Assert(utils::InternalError, name == mVariableName,
		       boost::format("Qss perturbation, invalid variable name: %1%") % name);
		
		reset(time, value);
		_reset = true;
	    }
	    ++it;
	}
	if (mState == POST) {
	    mState = POST2;
	    mSigma = devs::Time(0);
	}
	else if (mState == RUN) {
	    if (_reset) mSigma = devs::Time(0);
	    else {
		// Mise � jour de la valeur
		mValue += (time - mLastTime).getValue()*mGradient;
		// Mise � jour des valeurs de variables externes
		if (mExternalVariableNumber > 1) {
		    for (unsigned int i = 0 ; i < mExternalVariableNumber ; i++)
			mExternalVariableValue[i] += (time - mLastTime).getValue()*mExternalVariableGradient[i];
		}
		mLastTime = time;
		mGradient = compute();    
		mSigma = devs::Time(0);
		mState = POST3;		
	    }
	}
    }
}

Value qss::processStateEvent(const StateEvent& event) const
{
    Assert(utils::InternalError, event.getPortName() == mVariableName,
	   boost::format("Qss model, invalid variable name: %1%") % event.getStringAttributeValue("name"));

    double e = (event.getTime() - mLastTime).getValue();

    return value::DoubleFactory::create(mValue+e*mGradient);
}

void qss::processInstantaneousEvent(const InstantaneousEvent& event,
				    const Time& time,
				    ExternalEventList& output) const
{
    Assert(utils::InternalError, event.getStringAttributeValue("name") == mVariableName,
	   boost::format("Qss model, invalid variable name: %1%") % event.getStringAttributeValue("name"));

    double e = (time - mLastTime).getValue();
    devs::ExternalEvent* ee = new devs::ExternalEvent("response");
      
    ee << devs::attribute("name", event.getStringAttributeValue("name"));
    ee << devs::attribute("value", mValue+e*mGradient);
    ee << devs::attribute("gradient", mGradient);
    output.addEvent(ee);
}

}} // namespace vle extension
