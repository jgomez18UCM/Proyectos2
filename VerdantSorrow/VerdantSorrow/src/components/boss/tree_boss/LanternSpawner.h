#pragma once

#include "../../../ecs/Component.h"

class Transform;
class CollisionManager;
class BossAtributos;
class FramedImage;

class LanternSpawner: public ecs::Component
{
public:
	//_CMPID_DECL_(ecs::_LANTERNSPAWNER);

		LanternSpawner(CollisionManager* collManager);
		~LanternSpawner();

	void initComponent() override;
	void createLantern(int x);


private:
	CollisionManager* collManager_;
	FramedImage* framedImg_;

	int lanternWidht_;
};

