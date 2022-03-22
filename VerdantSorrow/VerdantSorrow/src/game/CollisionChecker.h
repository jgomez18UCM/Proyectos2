#pragma once
#include "../ecs/Entity.h"
#include "../ecs/Manager.h"
class CollisionManager;
class CollisionChecker
{
public:
	CollisionChecker(CollisionManager* colManager, ecs::Manager* mngr);
	virtual ~CollisionChecker();
	void collisionsFrogScene();
private:
	CollisionManager* colManager_;
	ecs::Manager* mngr_;
};

