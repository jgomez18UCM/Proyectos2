#include "TutorialSpawnRoot.h"

#include "../../sdlutils/SDLUtils.h"
#include "../../ecs/Entity.h"
#include "../../ecs/Manager.h"
#include "../Transform.h"
#include "../RectangleCollider.h"
#include "../../game/CollisionManager.h"
#include "../Image.h"
#include "../tutorial/TutorialRootMovement.h"
#include "../boss/bossAtributos.h"


TutorialSpawnRoot::TutorialSpawnRoot(CollisionManager* colManager) : colManager_(colManager), framedImg_(), 
								rootWidth_(60), rootPos_(900),lastTime_()
{
}

TutorialSpawnRoot::~TutorialSpawnRoot()
{
}

void TutorialSpawnRoot::initComponent()
{
	lastTime_ = mngr_->addTimer();
}


void TutorialSpawnRoot::update() {

	if (lastTime_->currTime() > 200) {

		if (rootPos_ <= 900) createRoot(rootPos_);
		rootPos_ -= 50;
		if (rootPos_ <= 500) rootPos_ = 1100;
		lastTime_->reset();
	}
}

void TutorialSpawnRoot::createRoot(int x)
{
	//Se crea la raiz
	auto Root = mngr_->addEntity();
	//Se a�aden los atributos del boss que est�n junto al transform
	Root->addComponent<BossAtributos>(1);
	auto RootTr = Root->addComponent<Transform>();
	auto RootX = x;
	auto RootY = sdlutils().height();
	//Se le dan las posiciones iniciales, velocidad, ancho y alto a la raiz
	RootTr->init(Vector2D(RootX, RootY), Vector2D(), 50, 800, 0.0f);
	//Se le a�ade un color inicial a la raiz
	Root->addComponent<Image>(&sdlutils().images().at("root"));

	//Se a�ade un collider a la raiz
	auto RootCollider = Root->addComponent<RectangleCollider>(RootTr->getWidth(), RootTr->getHeight());
	RootCollider->setIsTrigger(true);
	//Se a�ade el collider al colliderGameManager
	colManager_->addCollider(RootCollider);
	//Se a�ade el movimiento vertical
	Root->addComponent<TutorialRootMovement>();
	Root->addToGroup(ecs::_BOSSELEMENTS_GRP);
}