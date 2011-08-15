/* Copyright (c) 2011
*
* Greg Hendrickson (LordGregGreg Back). All rights reserved.
*
* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
*
*   1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above
*      copyright notice, this list of conditions and the following
*      disclaimer in the documentation and/or other materials provided
*      with the distribution.
*   3. Neither the name Modular Systems nor the names of its contributors
*      may be used to endorse or promote products derived from this
*      software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY LGG AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "llviewerprecompiledheaders.h"
#include "lggfriendsgroups.h"
#include "lggfriendsgroupsfloater.h"

#include "llagentdata.h"
#include "llcommandhandler.h"
#include "llfloater.h"
#include "lluictrlfactory.h"
#include "llagent.h"
#include "llpanel.h"
#include "llbutton.h"
#include "llcolorswatch.h"
#include "llcombobox.h"
#include "llview.h"
#include "llpanelphoenix.h"
#include "llhttpclient.h"
#include "llbufferstream.h"
#include "llcheckboxctrl.h"
#include "llviewercontrol.h"

#include "llui.h"
#include "llcontrol.h"
#include "llscrolllistctrl.h"
#include "llscrollingpanellist.h"
#include "lggautocorrect.h"
#include "llfilepicker.h"
#include "llfile.h"
#include "llsdserialize.h"
#include "llfloaterchat.h"
#include "llchat.h"
#include "llviewerinventory.h"
#include "llinventorymodel.h"
#include "llhost.h"
#include "llassetstorage.h"
#include "roles_constants.h"
#include "llviewertexteditor.h"

#include "llappviewer.h"
#include "llavatarnamecache.h"
#include "lluuid.h"
#include "llavatarname.h"
#include "llcallingcard.h"
#include "lluserrelations.h"
#include "llfloateravatarinfo.h"
#include "llimview.h"
#include "llviewermessage.h"
#include "llfloaterworldmap.h"
#include "llstring.h"

class lggFriendsGroupsFloater;
class lggFriendsGroupsFloater : public LLFloater, public LLFloaterSingleton<lggFriendsGroupsFloater>, public LLFriendObserver
{
public:
	lggFriendsGroupsFloater(const LLSD& seed);
	virtual ~lggFriendsGroupsFloater();

	virtual void changed(U32 mask);
	BOOL postBuild(void);
	void setData(void * data);
	BOOL handleMouseDown(S32 x,S32 y,MASK mask);
	void update();
	BOOL handleRightMouseDown(S32 x,S32 y,MASK mask);
	BOOL handleHover(S32 x, S32 y, MASK mask);
	
	BOOL generateCurrentList();
	void draw();
	void drawScrollBars();
	void drawRightClick();
	void toggleSelect(int pos);
	static BOOL compareAv(LLUUID av1, LLUUID av2);

	static void onClickSettings(void* data);
	static void onClickNew(void* data);
	static void onClickDelete(void* data);

	static void onBackgroundChange(LLUICtrl* ctrl, void* userdata);

	static void onNoticesChange(LLUICtrl* ctrl, void* userdata);
	static void onCheckBoxChange(LLUICtrl* ctrl, void* userdata);

	void updateGroupsList();
	void updateGroupGUIs();

	static void onSelectGroup(LLUICtrl* ctrl, void* userdata);

	LLComboBox* groupsList;
	LLColorSwatchCtrl * groupColorBox;
	LLCheckBoxCtrl* noticeBox;
	LLRect contextRect;

	LLPanelPhoenix * phpanel;
	static lggFriendsGroupsFloater* sInstance;
private:
	
	S32 mouse_x;
	S32 mouse_y;
	F32 hovered;
	S32 maxSize;
	std::vector<S32> selected;
	std::vector<LLUUID> currentList;
	S32 scrollLoc;
	BOOL showRightClick;
	BOOL justClicked;
};

lggFriendsGroupsFloater* lggFriendsGroupsFloater::sInstance;

lggFriendsGroupsFloater::~lggFriendsGroupsFloater()
{
	LLAvatarTracker::instance().removeObserver(sInstance);
	sInstance = NULL;
}
lggFriendsGroupsFloater::lggFriendsGroupsFloater(const LLSD& seed)
:mouse_x(0),mouse_y(900),hovered(0.f),justClicked(FALSE),scrollLoc(0),showRightClick(FALSE),maxSize(0)
{
	if(sInstance)delete sInstance;
	sInstance = this;
	selected.clear();
	currentList.clear();
	LLAvatarTracker::instance().addObserver(this);
	
	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_friendsgroups.xml");
}
void lggFriendsGroupsFloater::changed(U32 mask)
{
	sInstance->generateCurrentList();
}
void lggFriendsGroupsFloater::onBackgroundChange(LLUICtrl* ctrl, void* userdata)
{
	LLColorSwatchCtrl* cctrl = (LLColorSwatchCtrl*)ctrl;

	if(cctrl)
	{
		std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);

		LGGFriendsGroups::getInstance()->setGroupColor(*currentGroup,cctrl->get());

	}

}
void lggFriendsGroupsFloater::onNoticesChange(LLUICtrl* ctrl, void* userdata)
{
	LLCheckBoxCtrl* cctrl = (LLCheckBoxCtrl*)ctrl;

	if(cctrl)
	{
		std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);

		LGGFriendsGroups::getInstance()->setNotifyForGroup(*currentGroup, ctrl->getValue().asBoolean());
		
	}

}
void lggFriendsGroupsFloater::onCheckBoxChange(LLUICtrl* ctrl, void* userdata)
{
	LLCheckBoxCtrl* cctrl = (LLCheckBoxCtrl*)ctrl;

	if(cctrl)
	{
		sInstance->generateCurrentList();
		
	}

}
void lggFriendsGroupsFloater::updateGroupsList()
{
	std::string currentGroup = gSavedSettings.getString("PhoenixFriendsGroupsSelectedGroup");
	LLComboBox * cb = groupsList;
	//if(	sInstance->groupsList != NULL) cb = sInstance->groupsList;
	
	cb->clear();
	cb->removeall();
	std::vector<std::string> groups = LGGFriendsGroups::getInstance()->getAllGroups();
	for(int i =0;i<groups.size();i++)
	{
		cb->add(groups[i],groups[i],ADD_BOTTOM,TRUE);
		
	}
	if((currentGroup=="")&&(groups.size()>0))
	{
		gSavedSettings.setString("PhoenixFriendsGroupsSelectedGroup",groups[0]);
		updateGroupGUIs();
		generateCurrentList();
	}else
	cb->setSimple(currentGroup);
}
void lggFriendsGroupsFloater::updateGroupGUIs()
{
	std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);
	
	groupColorBox->set(LGGFriendsGroups::getInstance()->getGroupColor(*currentGroup),TRUE);	
	
	groupsList->setSimple(*currentGroup);
	
	noticeBox->set(LGGFriendsGroups::getInstance()->getNotifyForGroup(*currentGroup));
}
void lggFriendsGroupsFloater::onSelectGroup(LLUICtrl* ctrl, void* userdata)
{
	LLComboBox* cctrl = (LLComboBox*)ctrl;

	if(cctrl)
	{
		gSavedSettings.setString("PhoenixFriendsGroupsSelectedGroup",cctrl->getSimple());
		sInstance->updateGroupGUIs();
		sInstance->selected.clear();

	}
	sInstance->generateCurrentList();
}
BOOL lggFriendsGroupsFloater::postBuild(void)
{
	groupsList= getChild<LLComboBox>("lgg_fg_groupCombo");
	groupsList->setCommitCallback(onSelectGroup);

	groupColorBox = getChild<LLColorSwatchCtrl>("colorswatch");
	
	childSetAction("lgg_fg_groupCreate",onClickNew,this);
	childSetAction("lgg_fg_groupDelete",onClickDelete,this);
	childSetAction("lgg_fg_openSettings",onClickSettings,this);

	groupColorBox->setCommitCallback(onBackgroundChange);

	noticeBox = getChild<LLCheckBoxCtrl>("lgg_fg_showNotices");
	noticeBox->setCommitCallback(onNoticesChange);

	getChild<LLCheckBoxCtrl>("lgg_fg_showOnline")->setCommitCallback(onCheckBoxChange);
	getChild<LLCheckBoxCtrl>("lgg_fg_showOffline")->setCommitCallback(onCheckBoxChange);
	getChild<LLCheckBoxCtrl>("lgg_fg_showOtherGroups")->setCommitCallback(onCheckBoxChange);
	getChild<LLCheckBoxCtrl>("lgg_fg_showAllFriends")->setCommitCallback(onCheckBoxChange);

	updateGroupsList();
	generateCurrentList();
	updateGroupGUIs();

	return true;
}



void lggFriendsGroupsFloater::setData(void * data)
{
	phpanel = (LLPanelPhoenix*)data;
}


void lggFriendsGroupsFloater::drawScrollBars()
{

}
void lggFriendsGroupsFloater::drawRightClick()
{
	std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);

	if(!sInstance->hasFocus())
	{
		showRightClick=FALSE;
		return;
	}
	int heightPer = 20;
	int width = 200;

	int extras = 5;//make sure we have room for the extra options
	BOOL canMap = FALSE;
	if(selected.size()==1)
	{
		extras+=5;//space,name,tp, profile, im
		//map
		if((LLAvatarTracker::instance().getBuddyInfo(currentList[selected[0]])->getRightsGrantedFrom())& LLRelationship::GRANT_MAP_LOCATION)
		{
			if(LLAvatarTracker::instance().getBuddyInfo(currentList[selected[0]])->isOnline())
			{
				extras+=1;
				canMap=TRUE;
			}
		}
	}
	if(selected.size()>1)
	{
		extras+=4;//name, conference, mass tp, space
	}

	std::vector<std::string> groups = LGGFriendsGroups::getInstance()->getAllGroups();
	int height = heightPer*(extras+groups.size());

	LLRect rec = sInstance->getChild<LLPanel>("draw_region")->getRect();
	gGL.color4fv(LLColor4(0,0,0,.5).mV);
	gl_rect_2d(rec);
	
	if((contextRect.mLeft+width)>rec.mRight)
	{
		contextRect.mLeft=rec.mRight-width;
	}
	if((contextRect.mTop- (height))<rec.mBottom)
	{
		contextRect.mTop= rec.mBottom+height;
	}	
	contextRect.setLeftTopAndSize(contextRect.mLeft,contextRect.mTop,width,height);
	LLRect bgRect;
	bgRect.setLeftTopAndSize(contextRect.mLeft-2,contextRect.mTop+2,width+4,contextRect.getHeight()+4);
	gGL.color4fv(LLColor4::black.mV);
	gl_rect_2d(bgRect);
	
	int top = contextRect.mTop;
	for(int i = 0;i<groups.size();i++)
	{
		LLRect addBackGround;
		addBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);		
		
		gGL.color4fv(LGGFriendsGroups::getInstance()->getGroupColor(groups[i]).mV);
		gl_rect_2d(addBackGround);
		if(addBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(addBackGround,FALSE);
			if(justClicked)
			{
				for(int v=0;v<selected.size();v++)
				{
					LLUUID afriend = currentList[selected[v]];
					LGGFriendsGroups::getInstance()->addFriendToGroup(
						afriend,groups[i]);
				}
				selected.clear();
			}
		}

		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string("Add to: "+groups[i])
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);

		top-=heightPer;
	}
	top-=heightPer;
	//draw remove button
	LLRect remBackGround;
	remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
	gGL.color4fv(LGGFriendsGroups::getInstance()->getGroupColor(*currentGroup).mV);
	gl_rect_2d(remBackGround);
	if(remBackGround.pointInRect(mouse_x,mouse_y))
	{
		//draw hover effect
		gGL.color4fv(LLColor4::yellow.mV);
		gl_rect_2d(remBackGround,FALSE);
		if(justClicked)
		{
			for(int v=0;v<selected.size();v++)
			{
				LLUUID afriend = currentList[selected[v]];
				LGGFriendsGroups::getInstance()->removeFriendFromGroup(
					afriend,*currentGroup);

				generateCurrentList();
			}

			selected.clear();
		}
	}

	LLFontGL::getFontSansSerif()->renderUTF8(
		std::string("Remove From: "+*currentGroup)
		, 0,
		contextRect.mLeft,
		top-(heightPer/2),
		LLColor4::white, LLFontGL::LEFT,
		LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);

	top-=heightPer;
	top-=heightPer;
	//specials
	if(selected.size()==1)
	{

		std::string avName("");
		LLAvatarName avatar_name;
		if(LLAvatarNameCache::get(currentList[selected[0]], &avatar_name))avName=avatar_name.getLegacyName();

		LLColor4 friendColor = LGGFriendsGroups::getInstance()->getFriendColor(currentList[selected[0]],"");
		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//no
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			avName
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		top-=heightPer;
		
		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		gGL.color4fv(friendColor.mV);
		gl_rect_2d(remBackGround);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//profileclick
				LLFloaterAvatarInfo::showFromDirectory(currentList[selected[0]]);
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string("View Profile")
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		top-=heightPer;

		//// map
		if(canMap)
		{
			remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
			gGL.color4fv(friendColor.mV);
			gl_rect_2d(remBackGround);
			if(remBackGround.pointInRect(mouse_x,mouse_y))
			{
				//draw hover effect
				gGL.color4fv(LLColor4::yellow.mV);
				gl_rect_2d(remBackGround,FALSE);
				if(justClicked)
				{
					//mapclick
					if( gFloaterWorldMap )
					{
						gFloaterWorldMap->trackAvatar(currentList[selected[0]],avName);
						LLFloaterWorldMap::show(NULL, TRUE);
					}

				}
			}
			LLFontGL::getFontSansSerif()->renderUTF8(
				std::string("Map Avatar")
				, 0,
				contextRect.mLeft,
				top-(heightPer/2),
				LLColor4::white, LLFontGL::LEFT,
				LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
			top-=heightPer;
		}

		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		gGL.color4fv(friendColor.mV);
		gl_rect_2d(remBackGround);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//im avatar
				char buffer[MAX_STRING];
				LLAvatarName avatar_name;
				if (LLAvatarNameCache::get(currentList[selected[0]], &avatar_name))
				{
					snprintf(buffer, MAX_STRING, "%s", avatar_name.getLegacyName().c_str());
					gIMMgr->setFloaterOpen(TRUE);
					gIMMgr->addSession(
						buffer,
						IM_NOTHING_SPECIAL,
						currentList[selected[0]]);
				}
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string("Instant Message")
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		top-=heightPer;

		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		gGL.color4fv(friendColor.mV);
		gl_rect_2d(remBackGround);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//offer tp click
				handle_lure(currentList[selected[0]]);
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string("Teleport Avatar")
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		
	}
	//group of avatars
	if(selected.size()>1)
	{
		LLColor4 groupColor = LGGFriendsGroups::getInstance()->getGroupColor(*currentGroup);
		LLDynamicArray<LLUUID> ids;
		for(int se=0;se<selected.size();se++)
		{
			ids.push_back(currentList[selected[se]]);
		}

		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//no
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string(llformat("All %d Selected",selected.size()))
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		top-=heightPer;

		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		gGL.color4fv(groupColor.mV);
		gl_rect_2d(remBackGround);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//confrence				
				gIMMgr->setFloaterOpen(TRUE);
				gIMMgr->addSession(std::string(*currentGroup+" Conference"), IM_SESSION_CONFERENCE_START, ids[0], ids);
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string(llformat("Start Conference Call (%d)",selected.size()))
			, 0, 
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		top-=heightPer;

		remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
		gGL.color4fv(groupColor.mV);
		gl_rect_2d(remBackGround);
		if(remBackGround.pointInRect(mouse_x,mouse_y))
		{
			//draw hover effect
			gGL.color4fv(LLColor4::yellow.mV);
			gl_rect_2d(remBackGround,FALSE);
			if(justClicked)
			{
				//mass tp
				handle_lure(ids);
			}
		}
		LLFontGL::getFontSansSerif()->renderUTF8(
			std::string(llformat("Send Mass TP (%d)",selected.size()))
			, 0,
			contextRect.mLeft,
			top-(heightPer/2),
			LLColor4::white, LLFontGL::LEFT,
			LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
		


	}

	top-=heightPer;
	top-=heightPer;
	remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
	if(remBackGround.pointInRect(mouse_x,mouse_y))
	{
		//draw hover effect
		gGL.color4fv(LLColor4::yellow.mV);
		gl_rect_2d(remBackGround,FALSE);
		if(justClicked)
			selected.clear();
	}
	LLFontGL::getFontSansSerif()->renderUTF8(
		std::string(llformat("Deselect All (%d)",selected.size()))
		, 0,
		contextRect.mLeft,
		top-(heightPer/2),
		LLColor4::white, LLFontGL::LEFT,
		LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
	top-=heightPer;

	remBackGround.setLeftTopAndSize(contextRect.mLeft,top,width,heightPer);
	if(remBackGround.pointInRect(mouse_x,mouse_y))
	{
		//draw hover effect
		gGL.color4fv(LLColor4::yellow.mV);
		gl_rect_2d(remBackGround,FALSE);
		if(justClicked)
		{
			std::vector<S32> newSelected;
			newSelected.clear();
			for(int pp=0;pp<currentList.size();pp++)
			{
				newSelected.push_back(pp);
			}
			selected=newSelected;
		}
	}
	LLFontGL::getFontSansSerif()->renderUTF8(
		std::string(llformat("Select All (%d)",currentList.size()-selected.size()))
		, 0,
		contextRect.mLeft,
		top-(heightPer/2),
		LLColor4::white, LLFontGL::LEFT,
		LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);

	if(justClicked)
	{
		showRightClick=FALSE;
		if(selected.size()==1)selected.clear();
	}
	justClicked=FALSE;	
}
void lggFriendsGroupsFloater::draw()
{
	LLFloater::draw();
	if(sInstance->isMinimized())return;

	LLFontGL* font = LLFontGL::getFontSansSerifSmall();
	LLFontGL* bigFont = LLFontGL::getFontSansSerifBig();
	LLFontGL* hugeFont = LLFontGL::getFontSansSerifHuge();


	static std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);
	static BOOL *textNotBg = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsColorizeText",&gSavedSettings,true);
	static BOOL *barNotBg = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsColorizeBar",&gSavedSettings,true);
	
		
	std::vector<LLUUID> workingList;
	workingList= currentList;
	int numberOfPanels = workingList.size();//45;

	LLRect topScroll = getChild<LLPanel>("top_region")->getRect();
	LLRect bottomScroll = getChild<LLPanel>("bottom_region")->getRect();
	LLRect rec  = getChild<LLPanel>("draw_region")->getRect();

	gGL.pushMatrix();
	gl_circle_2d(rec.getCenterX(),rec.getCenterY(),2.0f,(S32)30,false);
	int bMag = 35;
	//kinda magic numbers to compensate for max bloom effect and stuff
	float sizeV = (F32)((rec.getHeight()-143)-(((F32)numberOfPanels)*1.07f)-0)/(F32)(numberOfPanels);
	maxSize=sizeV+bMag;
	if(sizeV<10)
	{
		//need scroll bars
		sizeV=10;
		if(this->hasFocus())
		{
			LLUIImage *arrowUpImage = LLUI::getUIImage("map_avatar_above_32.tga");
			LLUIImage *arrowDownImage = LLUI::getUIImage("map_avatar_below_32.tga");
			LLColor4 active = LGGFriendsGroups::getInstance()->getGroupColor(*currentGroup);
			LLColor4 unactive = LGGFriendsGroups::toneDownColor(active,.5);

			LLColor4 useColor = unactive;
			if(mouse_y<topScroll.mTop&& mouse_y > topScroll.mBottom)
			{
				useColor=active;
				scrollLoc-=llclamp((numberOfPanels/16),20,70);
			}
			if(scrollLoc>0)
			{
				gGL.color4fv(useColor.mV);
				gl_rect_2d(topScroll,true);
			}		
			
			int x=topScroll.mLeft;
			if(scrollLoc>0)
			for(;x<topScroll.mRight-topScroll.getHeight();x+=(30+topScroll.getHeight()))
			{
				gl_draw_scaled_image_with_border(x,
					topScroll.mBottom,
					topScroll.getHeight(),topScroll.getHeight(),
					arrowUpImage->getImage(),
					useColor,
					FALSE); 
			}
			int maxS =((numberOfPanels*11)+200-(rec.getHeight()));
			useColor=unactive;
			if(mouse_y<bottomScroll.mTop && mouse_y > bottomScroll.mBottom)
			{
				useColor=active;
				scrollLoc+=llclamp((numberOfPanels/16),20,70);
			}
			if(scrollLoc<maxS)
			{
				gGL.color4fv(useColor.mV);
				gl_rect_2d(bottomScroll,true);
			}
			if(scrollLoc<maxS)
				for(x=bottomScroll.mLeft;x<bottomScroll.mRight-bottomScroll.getHeight();x+=(30+bottomScroll.getHeight()))
				{
					gl_draw_scaled_image_with_border(x,
						bottomScroll.mBottom,
						bottomScroll.getHeight(),topScroll.getHeight(),
						arrowDownImage->getImage(),
						useColor,
						FALSE); 
				}
			scrollLoc = llclamp(scrollLoc,0,maxS);
		}
	}
	else
	{
		scrollLoc=0;
	}
	float top=rec.mTop+scrollLoc;//sizeV+12;
	if(mouse_y<15)mouse_y=15;
	for(int p = 0; p < numberOfPanels ; p++)
	{
		float thisSize = sizeV;
		float pi = 3.1415f;
		float piOver2Centered = pi/2+( (top-((F32)(sizeV+(40))/2.0f)-mouse_y)*.01);
		float bubble =sin((float)llclamp(piOver2Centered,0.0f,pi))*bMag;
		thisSize+=bubble;

		if((top-thisSize)<rec.mBottom)continue; 
		if((top-thisSize)>rec.mTop){}
		else
		{
			if((top)>rec.mTop){top=rec.mTop;}//draw as much as the top one as we can

			LLRect box;
			box.setLeftTopAndSize(rec.mLeft+(bMag/2)+5-(bubble/2),llceil(top+.00001),(rec.getWidth()-bMag-10)+(bubble/1),(int)llfloor(thisSize+.00001f));

			BOOL hoveringThis=FALSE;
			if(top>mouse_y && (top-thisSize)<mouse_y)
			{
				hoveringThis=TRUE;
				
			}
			BOOL iAMSelected = FALSE;
			for(int i = 0; i < (int)selected.size();i++)
			{	
				if(selected[i]==p)
				{
					iAMSelected=TRUE;
				}
			}

			LLUUID agent_id = workingList[p];

			BOOL *showOtherGroups = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsShowOtherGroups", &gSavedSettings, true);
			std::string *cg = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);
			std::vector<std::string> groupsIsIn;				
			groupsIsIn= LGGFriendsGroups::getInstance()->getFriendGroups(agent_id);

			LLColor4 color = LGGFriendsGroups::getInstance()->getGroupColor(*cg);
			if(!LGGFriendsGroups::getInstance()->isFriendInGroup(agent_id,*cg))
				color = LGGFriendsGroups::getInstance()->getDefaultColor();
			if(*showOtherGroups)color = LGGFriendsGroups::getInstance()->
				getFriendColor(agent_id,*cg);

			if(!iAMSelected)
				color = LGGFriendsGroups::toneDownColor(color,((F32)bubble)/((F32)bMag));
			
			gGL.color4fv(color.mV);			
			if(!(*barNotBg) && !(*textNotBg))
			{
				gl_rect_2d(box);
			}else
			{
				LLRect smallBox = box;
				smallBox.setLeftTopAndSize(box.mLeft,box.mTop,10+(bubble/2),box.getHeight());
				gl_rect_2d(smallBox);
				smallBox.setLeftTopAndSize(box.mLeft+10+(bubble/2),box.mTop,box.getWidth()-(10+(bubble/2)),box.getHeight());
				gGL.color4fv(LGGFriendsGroups::toneDownColor(LGGFriendsGroups::getInstance()->getDefaultColor(),((F32)bubble)/((F32)bMag)).mV);
				gl_rect_2d(smallBox);
			}

			//draw over lays (other group names)
			if(box.getHeight()>25)
			{
				int breathingRoom = 0;
				if(box.getHeight()>35)breathingRoom=4;

				int w =box.mLeft+breathingRoom;
				int sizePerOGroup = 40;
				for(int gr=0;gr<groupsIsIn.size();gr++)
				{
					std::string oGroupName = groupsIsIn[gr];
					sizePerOGroup=
						LLFontGL::getFontSansSerifSmall()->getWidth(oGroupName)+8;
					LLColor4 oGroupColor = LGGFriendsGroups::getInstance()->getGroupColor(oGroupName);
					LLRect oGroupArea;
					oGroupArea.setLeftTopAndSize(w,box.mBottom+12+breathingRoom,sizePerOGroup,12+(breathingRoom/2));
					gGL.color4fv(oGroupColor.mV);			
					gl_rect_2d(oGroupArea);
					gGL.color4fv(LLColor4(1,1,1,.5).mV);			
					gl_rect_2d(oGroupArea,FALSE);
					LLFontGL::getFontSansSerifSmall()->renderUTF8(
						oGroupName
						, 0,
						w+4,
						box.mBottom+breathingRoom,
						LLColor4::white, LLFontGL::LEFT,
						LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
					w+=sizePerOGroup+5;

				}
			}
			//draw icons
			//not to small if we can, but not to big, but still have a good zoom effect
			int size =llclamp(thisSize+(bubble/2),
				llmax(10.0f,llmin((((F32)box.getHeight())/1.0f),20.0f)),
				llmin(20+(bubble/2),thisSize+(bubble/2)));

			std::string toolTipText="Friend is Unselected";
			std::string toDisplayToolTipText="";
			int xLoc = box.mRight-size;
			LLUIImage *selectedImage = LLUI::getUIImage("checkbox_enabled_false.tga");
			if(iAMSelected)
			{
				toolTipText="Friend is Selected";
				selectedImage = LLUI::getUIImage("checkbox_enabled_true.tga");
			}
			LLRect imageBox;

			gGL.color4fv(LLColor4::white.mV);
			
			imageBox.setLeftTopAndSize(xLoc,(box.getHeight()/2)+0+box.mBottom+(size/2),size,size);			
			gl_draw_scaled_image_with_border(imageBox.mLeft,imageBox.mBottom,
				imageBox.getWidth(),imageBox.getHeight(),
				selectedImage->getImage(),
				LLColor4::white,
				FALSE);
			if(imageBox.pointInRect(mouse_x,mouse_y))
			{
				gl_rect_2d(imageBox,FALSE);
				toDisplayToolTipText=toolTipText;
			}

			LLUIImage *onlineImage = LLUI::getUIImage("icon_avatar_online.tga");
			toolTipText = "Friend is Online";
			if(!LLAvatarTracker::instance().getBuddyInfo(agent_id)->isOnline())
			{
				onlineImage = LLUI::getUIImage("icon_avatar_offline.tga");
				toolTipText="Friend is Offline";
			}
			
			imageBox.setLeftTopAndSize(xLoc-=(1+size),(box.getHeight()/2)+0+box.mBottom+(size/2),size,size);			
			gl_draw_scaled_image_with_border(imageBox.mLeft,imageBox.mBottom,
				imageBox.getWidth(),imageBox.getHeight(),
				onlineImage->getImage(), 
				LLColor4::white,
				FALSE);
			if(imageBox.pointInRect(mouse_x,mouse_y))
			{
				toDisplayToolTipText=toolTipText;
			}

			////im button
			LLUIImage *imImage = LLUI::getUIImage("icn_chatbar.tga");

			imageBox.setLeftTopAndSize(xLoc-=(1+size),(box.getHeight()/2)+0+box.mBottom+(size/2),size,size);			
			gl_draw_scaled_image_with_border(imageBox.mLeft,imageBox.mBottom,
				imageBox.getWidth(),imageBox.getHeight(),
				imImage->getImage(),
				LLColor4::white,
				FALSE); 
			if(imageBox.pointInRect(mouse_x,mouse_y))
			{
				gl_rect_2d(imageBox,FALSE);				
				toDisplayToolTipText="Instant Message";
				if(justClicked&&!showRightClick)
				{
					justClicked=FALSE;
					char buffer[MAX_STRING];
					// [Ansariel/Henri: Display name support]
					// snprintf(buffer, MAX_STRING, "%s", avlist->mAvatars[agent_id].getName().c_str());
					LLAvatarName avatar_name;
					if (LLAvatarNameCache::get(agent_id, &avatar_name))
					{
						snprintf(buffer, MAX_STRING, "%s", avatar_name.getLegacyName().c_str());
						gIMMgr->setFloaterOpen(TRUE);
						gIMMgr->addSession(
							buffer,
							IM_NOTHING_SPECIAL,
							agent_id);
					}
				}
			}
			//profile button
			LLUIImage *profileImage = LLUI::getUIImage("map_infohub.tga");
			imageBox.setLeftTopAndSize(xLoc-=(1+size),(box.getHeight()/2)+0+box.mBottom+(size/2),size,size);			
			gl_draw_scaled_image_with_border(imageBox.mLeft,imageBox.mBottom,
				imageBox.getWidth(),imageBox.getHeight(),
				profileImage->getImage(),
				LLColor4::white,
				FALSE); 
			if(imageBox.pointInRect(mouse_x,mouse_y))
			{
				gl_rect_2d(imageBox,FALSE);
				toDisplayToolTipText="View Profile";
				if(justClicked&&!showRightClick)
				{
					justClicked=FALSE;
					LLFloaterAvatarInfo::showFromDirectory(agent_id);
				}
			}
			//draw hover and selected			
			if(iAMSelected)
			{
				gGL.color4fv(LLColor4(1,1,1,1.0).mV);
				gl_rect_2d(box,FALSE);
				//gGL.color4fv(LLColor4(1,1,1,.7).mV);
				//gl_circle_2d(box.mRight-(box.getHeight()/2),box.mTop-(box.getHeight()/2),box.getHeight()/2,20,TRUE);

			}
			if(hoveringThis)
			{
				gGL.color4fv(LLColor4::yellow.mV);
				gl_rect_2d(box,FALSE);
				if(justClicked&&!showRightClick)
					toggleSelect(p);
				
				if(showRightClick)
				{
					if(selected.size()<1)
						toggleSelect(p);
				}
				
			}

			//draw tooltip
			if(toDisplayToolTipText!="")
			{
				int tsize = LLFontGL::getFontSansSerifSmall()->getWidth(toDisplayToolTipText)+8;
				LLRect toolRect;toolRect.setLeftTopAndSize(mouse_x-tsize,mouse_y+16,tsize,16);
				gGL.color4fv(LLColor4::black.mV);
				gl_rect_2d(toolRect);
				//gGL.color4fv(LLColor4::yellow.mV);
				gl_rect_2d(toolRect,FALSE);
				LLFontGL::getFontSansSerifSmall()->renderUTF8(toDisplayToolTipText
					, 0,toolRect.mLeft+4,
					toolRect.mBottom+2,
					LLColor4::white, LLFontGL::LEFT,
					LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
			}

			if(thisSize>8)//draw name
			{
				std::string text="";
				LLAvatarName avatar_name;
				if (LLAvatarNameCache::get(agent_id, &avatar_name))
				{
					std::string fullname;
					static S32* sPhoenixNameSystem = rebind_llcontrol<S32>("PhoenixNameSystem", &gSavedSettings, true);
					switch (*sPhoenixNameSystem)
					{
					case 0 : fullname = avatar_name.getLegacyName(); break;
					case 1 : fullname = (avatar_name.mIsDisplayNameDefault? avatar_name.mDisplayName : avatar_name.getCompleteName()); break;
					case 2 : fullname = avatar_name.mDisplayName; break;
					default : fullname = avatar_name.getCompleteName(); break;
					}

					text+="" +fullname;
				}
				LLFontGL* useFont = font;
				if(thisSize>14)useFont = bigFont;
				if(thisSize>25)useFont = hugeFont;
				
				int roomForBar = 0;
				if((*barNotBg)||(*textNotBg))roomForBar=10+(bubble/2);

				LLColor4 nameTextColor = LLColor4::white;
				if((*textNotBg)&(groupsIsIn.size()>0))nameTextColor=LGGFriendsGroups::toneDownColor(color,1.0f);

				useFont->renderUTF8(
					text
					, 0,
					box.mLeft+roomForBar,
					top-(thisSize/2),
					nameTextColor, LLFontGL::LEFT,
					LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);

				//useFont->renderUTF8(llformat(" %d ",p),0,box.mLeft+0,top-(thisSize/2),LLColor4::white,LLFontGL::LEFT,
				//	LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);
			}

		}

		top-=(thisSize+1);

	}

	///mouse
	if(mouse_x!=0 && mouse_y!=0 && mouse_y<rec.mTop && (gFrameTimeSeconds-2<hovered))
	{
		gGL.color4fv(LLColor4::black.mV);
		//gl_circle_2d(mouse_x,mouse_y,20.0f,((S32)(gFrameTimeSeconds))%4 + 3,false);
	}

	/*font->renderUTF8(
		llformat("Mouse is at %d , %d scoll lock is %d height is %d panels is %d",
		mouse_x,mouse_y,scrollLoc,rec.getHeight(),numberOfPanels)
		, 0,
		rec.mLeft,
		rec.mBottom+0,
		LLColor4::white, LLFontGL::LEFT,
		LLFontGL::BASELINE, LLFontGL::DROP_SHADOW);*/

	if(showRightClick)drawRightClick();

	gGL.popMatrix();
	justClicked=FALSE;
}
void lggFriendsGroupsFloater::toggleSelect(int pos)
{
	justClicked=FALSE;
	bool found = false;
	for(int i = 0; i < (int)selected.size();i++)
	{
		if(selected[i]==pos)
			found=true;
	}
	if(!found)
		selected.push_back(pos);
	else
	{
		std::vector<S32> newList;
		newList.clear();
		for(int i = 0; i < (int)selected.size();i++)
		{
			if(selected[i]!=pos)
				newList.push_back(selected[i]);
		}
		selected=newList;
	}
	
}
BOOL lggFriendsGroupsFloater::handleMouseDown(S32 x,S32 y,MASK mask)
{
	sInstance->justClicked=true;
	return LLFloater::handleMouseDown(x,y,mask);
}
BOOL lggFriendsGroupsFloater::handleRightMouseDown(S32 x,S32 y,MASK mask)
{
	if(!showRightClick)
	{
		contextRect.setLeftTopAndSize(x,y,2,2);
		showRightClick=TRUE;
	}else
	{
		justClicked=TRUE;
	}
	return LLFloater::handleRightMouseDown(x,y,mask);
}
BOOL lggFriendsGroupsFloater::handleHover(S32 x,S32 y,MASK mask)
{
	mouse_x=x;
	mouse_y=y;
	hovered=gFrameTimeSeconds;
	return LLFloater::handleHover(x,y,mask);
}
BOOL lggFriendsGroupsFloater::compareAv(LLUUID av1, LLUUID av2)
{
	std::string avN1("");
	std::string avN2("");
	LLAvatarName avatar_name;
	if (LLAvatarNameCache::get(av1, &avatar_name))
	{
		std::string fullname;
		static S32* sPhoenixNameSystem = rebind_llcontrol<S32>("PhoenixNameSystem", &gSavedSettings, true);
		switch (*sPhoenixNameSystem)
		{
		case 0 : fullname = avatar_name.getLegacyName(); break;
		case 1 : fullname = (avatar_name.mIsDisplayNameDefault? avatar_name.mDisplayName : avatar_name.getCompleteName()); break;
		case 2 : fullname = avatar_name.mDisplayName; break;
		default : fullname = avatar_name.getCompleteName(); break;
		}

		avN1=fullname;
	}
	if (LLAvatarNameCache::get(av2, &avatar_name))
	{
		std::string fullname;
		static S32* sPhoenixNameSystem = rebind_llcontrol<S32>("PhoenixNameSystem", &gSavedSettings, true);
		switch (*sPhoenixNameSystem)
		{
		case 0 : fullname = avatar_name.getLegacyName(); break;
		case 1 : fullname = (avatar_name.mIsDisplayNameDefault? avatar_name.mDisplayName : avatar_name.getCompleteName()); break;
		case 2 : fullname = avatar_name.mDisplayName; break;
		default : fullname = avatar_name.getCompleteName(); break;
		}

		avN2=fullname;
	}
	LLStringUtil::toLower(avN2);
	LLStringUtil::toLower(avN1);
	
	return (avN1.compare(avN2))<0;
}
BOOL lggFriendsGroupsFloater::generateCurrentList()
{
	static BOOL *showOnline = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsShowOnline", &gSavedSettings, true);
	static BOOL *showOffline = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsShowOffline", &gSavedSettings, true);
	static BOOL *yshowAllFriends = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsShowAllFriends", &gSavedSettings, true);
	//static BOOL *showOtherGroups = rebind_llcontrol<BOOL>("PhoenixFriendsGroupsShowOtherGroups", &gSavedSettings, true);
	static std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);

	currentList.clear();
	std::map<LLUUID, LLRelationship*>::iterator p;
	std::map<LLUUID, LLRelationship*> friends;
	LLAvatarTracker::instance().copyBuddyList(friends);

	for(p = friends.begin(); 
		p != friends.end(); p++)
	{
		LLRelationship* relation = p->second;
		if((! (*showOnline))&&(relation->isOnline()))continue;
		if((! (*showOffline))&&(!relation->isOnline()))continue;
		if((! (*yshowAllFriends)&&(!LGGFriendsGroups::getInstance()->isFriendInGroup(p->first,*currentGroup))))continue;

		currentList.push_back(p->first);
	}
	std::sort(currentList.begin(),currentList.end(),&lggFriendsGroupsFloater::compareAv);
	return TRUE;
}
void lggFriendsGroupsFloater::onClickDelete(void* data)
{
	std::string *currentGroup = rebind_llcontrol<std::string>("PhoenixFriendsGroupsSelectedGroup", &gSavedSettings, true);

	LGGFriendsGroups::getInstance()->deleteGroup(*currentGroup);
	gSavedSettings.setString("PhoenixFriendsGroupsSelectedGroup","");
	sInstance->updateGroupsList();
}
void lggFriendsGroupsFloater::onClickNew(void* data)
{
	LLLineEditor* line =sInstance->getChild<LLLineEditor>("lgg_fg_groupNewName");
	std::string text = line->getText();
	if(text!="")
		LGGFriendsGroups::getInstance()->addGroup(text);

	sInstance->updateGroupsList();
}
void lggFriendsGroupsFloater::onClickSettings(void* data)
{
	lggFriendsGroupsFloaterStart::showSettings(TRUE);
}
class lggFriendsGroupsFloaterSettings;
class lggFriendsGroupsFloaterSettings : public LLFloater, public LLFloaterSingleton<lggFriendsGroupsFloaterSettings>
{
public:
	lggFriendsGroupsFloaterSettings(const LLSD& seed);
	virtual ~lggFriendsGroupsFloaterSettings();

	BOOL postBuild(void);
	static void onClickOk(void* data);
	static void onDefaultBackgroundChange(LLUICtrl* ctrl, void* userdata);
	static lggFriendsGroupsFloaterSettings* sSettingsInstance;
};


lggFriendsGroupsFloaterSettings* lggFriendsGroupsFloaterSettings::sSettingsInstance;

lggFriendsGroupsFloaterSettings::~lggFriendsGroupsFloaterSettings()
{
	sSettingsInstance = NULL;
}
lggFriendsGroupsFloaterSettings::lggFriendsGroupsFloaterSettings(const LLSD& seed)
{
	if(sSettingsInstance)delete sSettingsInstance;
	sSettingsInstance= this;
	
	if (getRect().mLeft == 0 
		&& getRect().mBottom == 0)
	{
		center();
	}
	LLUICtrlFactory::getInstance()->buildFloater(this, "floater_friendsgroups_settings.xml");
}
BOOL lggFriendsGroupsFloaterSettings::postBuild(void)
{
	childSetAction("lgg_fg_okButton",onClickOk,this);
	getChild<LLColorSwatchCtrl>("colordefault")->setCommitCallback(onDefaultBackgroundChange);
	getChild<LLColorSwatchCtrl>("colordefault")->set(LGGFriendsGroups::getInstance()->getDefaultColor());
	return TRUE;
}
void lggFriendsGroupsFloaterSettings::onDefaultBackgroundChange(LLUICtrl* ctrl, void* userdata)
{
	LLColorSwatchCtrl* cctrl = (LLColorSwatchCtrl*)ctrl;
	if(cctrl)
	{
		LGGFriendsGroups::getInstance()->setDefaultColor(cctrl->get());
	}
}
void lggFriendsGroupsFloaterSettings::onClickOk(void* data)
{
	sSettingsInstance->close();
}
void lggFriendsGroupsFloaterStart::show(BOOL showin,void * data)
{
	if(showin)
	{
		lggFriendsGroupsFloater* fg_floater = lggFriendsGroupsFloater::showInstance();
		fg_floater->setData(data);
	}
}
void lggFriendsGroupsFloaterStart::showSettings(BOOL showin)
{
	if(showin)
	{
		/*lggFriendsGroupsFloaterSettings* settings_floater =*/ lggFriendsGroupsFloaterSettings::showInstance();
	}
}