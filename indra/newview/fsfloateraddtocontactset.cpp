/*
 * @file fsfloateraddtocontactset.cpp
 * @brief Add an avatar to a contact set
 *
 * (C) 2013 Cinder Roxley @ Second Life <cinder.roxley@phoenixviewer.com>
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "llviewerprecompiledheaders.h"
#include "fsfloateraddtocontactset.h"

#include "fscontactsfloater.h"
#include "fspanelcontactsets.h"
#include "llnotificationsutil.h"
#include "llslurl.h"
#include <boost/foreach.hpp>

FSFloaterAddToContactSet::FSFloaterAddToContactSet(const LLSD& target)
:	LLFloater(target)
,	mContactSetsCombo(NULL)
{
	mAgentID = target.asUUID();
}

BOOL FSFloaterAddToContactSet::postBuild()
{
	childSetTextArg("textfield", "[NAME]", LLSLURL("agent", mAgentID, "inspect").getSLURLString());
	
	mContactSetsCombo = getChild<LLComboBox>("contact_sets");
	populateContactSets();
	
	childSetAction("add_btn",	boost::bind(&FSFloaterAddToContactSet::onClickAdd, this));
	childSetAction("cancel_btn", boost::bind(&FSFloaterAddToContactSet::onClickCancel, this));
	
	return TRUE;
}

void FSFloaterAddToContactSet::onClickAdd()
{
	const std::string set = mContactSetsCombo->getSimple();
	if (!LLAvatarTracker::instance().isBuddy(mAgentID))
		LGGContactSets::getInstance()->addNonFriendToList(mAgentID);
	LGGContactSets::getInstance()->addFriendToGroup(mAgentID, set);
	LLSD args;
	args["NAME"] = LLSLURL("agent", mAgentID, "inspect").getSLURLString();
	args["SET"] = set;
	LLNotificationsUtil::add("AddToContactSetSuccess", args);
	closeFloater();
	
	// Refresh the set list if it's there
	FSPanelContactSets* panel = dynamic_cast<FSPanelContactSets*>(FSFloaterContacts::findInstance()->getPanelByName("contact_sets_panel"));
	if (panel)
		panel->refreshSetList();
}

void FSFloaterAddToContactSet::onClickCancel()
{
	closeFloater();
}

void FSFloaterAddToContactSet::populateContactSets()
{
	if (!mContactSetsCombo) return;
	
	mContactSetsCombo->clear();
	std::vector<std::string> contact_sets = LGGContactSets::getInstance()->getAllGroups();
	if (contact_sets.empty())
	{
		mContactSetsCombo->add(getString("no_sets"), LLSD(NULL));
		getChild<LLButton>("add_btn")->setEnabled(FALSE);
	}
	else
	{
		BOOST_FOREACH(const std::string& set_name, contact_sets)
		{
			mContactSetsCombo->add(set_name);
		}
	}
}
