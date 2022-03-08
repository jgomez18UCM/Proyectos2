#pragma once
#include "../../ecs/Component.h"
#include "../../game/CollisionManager.h"
#include "../../ecs/Entity.h"
class NpcCtrl : public ecs::Component
{

public:
	__CMPID_DECL__(ecs::_NPC_CTRL)
	NpcCtrl(CollisionManager* colManager, Entity* dialogBox);
	~NpcCtrl() {};

	void initComponent() override;
	void update() override;

protected:
	Entity* dialogBox_;
	CollisionManager* colMan_;
	RectangleCollider* col_;

	bool canTalk;
};
