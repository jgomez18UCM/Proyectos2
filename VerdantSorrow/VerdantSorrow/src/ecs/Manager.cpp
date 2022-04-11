// This file is part of the course TPV2@UCM - Samir Genaim

#include "Manager.h"
#include "../game/FrogScene.h"
#include "../game/Hub.h"
#include "../game/TreeScene.h"
#include "../game/FinalBossScene.h"
#include "../game/TutorialScene.h"
namespace ecs {

Manager::Manager() :
		ents_(), //
		hdlrs_(), //
		entsByGroup_(), //
		debug_(false)
{
	// we reserve space for 100 entities, just to avoid resizing. You
	// can adjust this number.
	//
	ents_.reserve(100);

	windowScaleHeight_ = sdlutils().windowHeight() / 1080.0f;
	windowScaleWidth_ = sdlutils().windowWidth() / 1920.0f;

	// for each group we reserve space for 100 entities,
	// just to avoid copies
	//
	for (auto &groupEntities : entsByGroup_) {
		groupEntities.reserve(100);
	}
}

Manager::~Manager() {

	// delete all entities
	//
	for (auto e : ents_)
		delete e;
}

void Manager::refresh() {

	// remove dead entities from the groups lists, and also those
	// do not belong to the group anymore
	for (ecs::grpId_type gId = 0; gId < ecs::maxGroupId; gId++) {
			auto &groupEntities = entsByGroup_[gId];
		groupEntities.erase(
				remove_if(groupEntities.begin(), groupEntities.end(),
						[gId](Entity *e) {
							return !e->isAlive() || !e->hasGroup(gId);
						}), groupEntities.end());
	}

	// remove all dead
	ents_.erase( //
			remove_if(ents_.begin(), ents_.end(), [](Entity *e) {
				if (e->isAlive()) {
					return false;
				} else {
					delete e;
					return true;
				}
			}), //
			ents_.end());

}

/*void Manager::changeScene(int n)
{
	switch (n)
	{
	case 0: {
		Hub h;
		h.init() ;
		//h.start();
		break;
	}
	case 1: {
		FrogScene f;
		f.init();
		//f.start();
		break;
	}
	case 2: {
		TreeScene t;
		t.init();
		//t.start();
		break;
	}
	case 3: {
		FinalBossScene t;
		t.init();
		//t.start();
		break;
	}
	case 4: {
		TutorialScene t;
		t.init();
		//t.start();
		break;
	}
	default:
		break;
	}

}*/

} // end of namespace
