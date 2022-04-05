// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <cassert>

#include "Component.h"
#include "ecs.h"
#include "Entity.h"
#include "../game/SceneManager.h"

namespace ecs {

class Manager {

public:
	Manager();
	virtual ~Manager();

	// Adding an entity simply creates an instance of Entity, adds
	// it to the list of entities and returns it to the caller.
	//
	inline Entity* addEntity() {

		// create and initialise the entity
		auto e = new Entity();
		e->setAlive(true);
		e->setActive(true);
		e->resetGroups();
		e->setContext(this);

		// add the entity 'e' to list of entities
		//
		// IMPORTANT NOTE:
		//
		// Currently we immediately add the entity to the list of entities 'ents_',
		// so we will actually see them in this 'frame' if we traverse the list of
		// entities afterwards!
		//
		// A better solution would be to add them to an auxiliary list, and
		// define a method 'flush()' that moves them from the auxiliary list
		// to 'ents_'. We will have to call 'flush()' in each iteration of the
		// main loop. This way we guarantee that entities that are added in one
		// 'frame' they will appear only in the next 'frame' -- I leave it as an
		// exercise for you ...
		//
		ents_.push_back(e);

		// return it to the caller
		//
		return e;
	}

	// returns the vector of all entities
	//
	inline const auto& getEntities() {
		return ents_;
	}

	// returns the vector of all entities for group 'gId'
	//
	inline const auto& getEntitiesByGroup(grpId_type gId) {
		assert(gId < ecs::maxGroupId);
		return entsByGroup_[gId];
	}

	// associates the entity 'e' to the handler 'hId'
	//
	inline void setHandler(hdlrId_type hId, Entity *e) {
		assert(hId < ecs::maxHdlrId);
		hdlrs_[hId] = e;
	}

	// returns the entity associated to the handler 'hId'
	//
	inline Entity* getHandler(hdlrId_type hId) {
		assert(hId < ecs::maxHdlrId);
		return hdlrs_[hId];
	}

	// call update of all entities
	//
	void update() {
		auto n = ents_.size();
		for (auto i = 0u; i < n; i++)
			ents_[i]->update();
	}

	// call render of all entities
	//
	void render() {
		if (SceneManager::scenes::Frog_ == sC().getScene()|| SceneManager::scenes::Tree_ == sC().getScene() 
			|| SceneManager::scenes::Eye_ == sC().getScene() || SceneManager::scenes::Hub_ == sC().getScene()) {
			//Renderización de todos los elementos del background
			auto background = getEntitiesByGroup(ecs::_BACKGROUND_GRP);
			auto e = background.size();
			for (auto i = 0; i < e; i++)
			{
				background[i]->render();
			}
			//Renderización de todos los elementos de la decoración del hub
			auto hubDecoration = getEntitiesByGroup(ecs::_HUB_DECORATION_GRP);
			e = hubDecoration.size();
			for (auto i = 0; i < e; i++)
			{
				hubDecoration[i]->render();
			}
			//Renderización de los bosses
			auto boss = getEntitiesByGroup(ecs::_BOSS_GRP);
			e = boss.size();
			for (auto i = 0; i < e; i++)
			{
				boss[i]->render();
			}
			//Renderización de todos los elementos adjuntados al boss ej:lámpara
			auto bossElements = getEntitiesByGroup(ecs::_BOSSELEMENTS_GRP);
			e = bossElements.size();
			for (auto i = 0; i < e; i++)
			{
				bossElements[i]->render();
			}
			//Renderización de todos los elementos que sean luces
			auto particles = getEntitiesByGroup(ecs::_PARTICLES_GRP);
			e = particles.size();
			for (auto i = 0; i < e; i++)
			{
				particles[i]->render();
			}
			//Renderización de todos los elementos que sean luces
			auto lights = getEntitiesByGroup(ecs::_LIGHTS_GRP);
			e = lights.size();
			for (auto i = 0; i < e; i++)
			{
				lights[i]->render();
			}
			//Renderización de todos los elementos de la UI
			getHandler(ecs::_PLAYER)->render();
			auto entitiesUI = getEntitiesByGroup(ecs::_UI_GRP);
			e = entitiesUI.size();
			for (auto i = 0; i < e; i++)
			{
				entitiesUI[i]->render();
			}
		}
		else{
			auto n = ents_.size();
			for (auto i = 0u; i < n; i++)
				ents_[i]->render();
		}
	}

	//call debug of all entities
	//
	void debug() {
		if (debug_) {
			auto n = ents_.size();
			for (auto i = 0u; i < n; i++)
				ents_[i]->debug();
		}
	}

	// eliminate dead entities (the implementation of this method
	// is in Manager.cpp, but we could also defined it here).
	//
	void refresh();

	//void changeScene(int n);

	//changes boolean that says if debug mode is on
	void setDebug(bool set) { debug_ = set; };

private:

	// class Entity is a friend so it can access addToGroup and removeFromGroup
	//
	friend Entity;

	// ** addToGroup and removeFromGroup do not check if 'e' is
	//    there for efficiency, etc. We suppose that they are
	//    called correctly. This is actually checked in the corresponding
	//    methods of Entity.

	// add the entity to the group 'gId'
	//
	void addToGroup(grpId_type gId, Entity *e) {
		entsByGroup_[gId].push_back(e);
	}

	std::vector<Entity*> ents_;
	std::array<Entity*, maxHdlrId> hdlrs_;
	std::array<std::vector<Entity*>, maxGroupId> entsByGroup_;

	//bool that says if debug mode is on
	bool debug_;
};

} // end of namespace
