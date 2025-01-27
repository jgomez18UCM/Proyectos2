#include "TreeScene.h"

#include "../sdlutils/InputHandler.h"
#include "../sdlutils/SDLUtils.h"
#include "../utils/Collisions.h"

#include "../ecs/Manager.h"
#include "../ecs/Entity.h"

#include "../components/RectangleRenderer.h"
#include "../components/RectangleCollider.h"
#include "../components/Transform.h"
#include "../components/FramedImage.h"
#include "../components/Image.h"
#include "../components/player/PlayerComponents.h"
#include "../components/boss/BossComponents.h"
#include "../components/boss/tree_boss/TreeAttackManager.h"
#include "../components/boss/tree_boss/RootWave.h"
#include "../components/boss/tree_boss/Root/RootMovement.h"
#include "../components/boss/tree_boss/RootSpawner.h"
#include "../components/boss/tree_boss/RootAutoAim.h"
#include "../components/boss/tree_boss/MeleeAttack.h"
#include "../components/boss/tree_boss/TreeMovement.h"
#include "../components/boss/tree_boss/LanternSpawner.h"
#include "../components//boss//tree_boss/LanternMovement.h"
#include "../components//boss//tree_boss/LanternCollisions.h"


#include "CollisionManager.h"
#include "SceneManager.h"
#include "../game/CollisionChecker.h"
#include "../components/fondos/ParticleSystem.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include "../components/CameraShake.h"


void TreeScene::init()
{
	//SDLUtils::init("Verdant Sorrow", 1280, 720, "resources/config/resources.json");
	Scene::init();
	//Para gestionar las colisiones
	CollisionManager* colManager = new CollisionManager();
	mngr_->setColManager(colManager);
	playerDying = false;
	Scene::background("fondo2");
	//Se crea el jugador 
	player = mngr_->addEntity();
	playerGenerator(colManager, player);

	camera_ = mngr_->addEntity();
	auto cameraTr = camera_->addComponent<Transform>();
	cameraTr->init(Vector2D(0, 0), Vector2D(0, 0), 0, 0, 0, 0, false);
	auto cShake = camera_->addComponent<CameraShake>(5, 10);
	mngr_->setHandler(ecs::_hdlr_CAMERA, camera_);

	treeGenerator(colManager);

	colCheck_ = new CollisionChecker(colManager, mngr_);

	playerDeathTimer_ = mngr_->addTimer();

	if (blackScreen_ == nullptr) createBlackScreen();

	else {

		delete blackScreen_;

		createBlackScreen();
	}

	blackScreenImg_->setAlpha(255);
	blackScreenImg_->fadeOut(3);
}


void TreeScene::update()
{
	auto bossHealth = mngr_->getHandler(ecs::_LANTERN)->getComponent<BossAtributos>();
	if (!playerAttribs_->isDefeated() && !bossHealth->isDefeated()) {
		mngr_->update();
		colCheck_->checkCollisions();
		mngr_->refresh();

		sdlutils().clearRenderer();
		mngr_->render();
#ifdef _DEBUG
		mngr_->debug();
#endif
		sdlutils().presentRenderer();

		if (playerDying) checkPlayerFinishedDying();
		else checkPlayerDied();
	}
	else {
		if (playerAttribs_->isDefeated()) sC().changeTreeEssenceState(true);
		if (bossHealth->isDefeated()) {
			sC().changeStatePlayerInBoss(false);
			Game::instance()->state_ = Game::State::TREEDEFEATED;

			ofstream myfile("resources/config/guardado.txt");
			if (myfile.is_open())
			{
				myfile << Game::State::TREEDEFEATED;
				myfile.close();
			}
			else cout << "Unable to open file";
		}

		treeMngr_->getMusicFirtPhase()->pauseChannel(0);
		setAble(false);
		sC().decideScene();
	}
}

void TreeScene::treeGenerator(CollisionManager* colManager) {

	tree_ = mngr_->addEntity();

	mngr_->setHandler(ecs::_TREEBOSS, tree_);
	tree_->addComponent<BossAtributos>(100);
	auto treeTr = tree_->addComponent<Transform>();
	auto treeH = 360;
	auto treeW = treeH * 1.105;
	auto treeX = sdlutils().width() / 4 * 3 - 80;
	auto treeY = sdlutils().height() - treeH - GROUND_HEIGHT;
	treeTr->init(Vector2D(treeX, treeY), Vector2D(), treeW, treeH, 0.0f);

	lanternGenerator(colManager, tree_, treeTr->getPos().getX(), treeTr->getPos().getY());

	tree_->addComponent<FramedImage>(&sdlutils().images().at("arbol_capa_idle"), 5, 6, (1000 / 30) * 25, 25, "arbol_capa_idle");

	//Se añade un collider al arbol
	auto treeCollider = tree_->addComponent<RectangleCollider>(treeTr->getWidth()/4, treeTr->getHeight()/1.2);
	treeCollider->setIsTrigger(true);
	colManager->addCollider(treeCollider);

	//IMPORTANTE: movimiento y spawner antes de los ataques
	tree_->addComponent<TreeMovement>(player->getComponent<Transform>(), 2.0f);
	tree_->addComponent<RootSpawner>(colManager);

	tree_->addComponent<RootWave>();
	tree_->addComponent<RootAutoAim>(player);
	tree_->addComponent<MeleeAttack>(100, treeH, colManager);

	//IMPORTANTE: attack manager al final
	treeMngr_ = tree_->addComponent<TreeAttackManager>(colManager);
	
	tree_->addToGroup(ecs::_BOSS_GRP);
}

void TreeScene::lanternGenerator(CollisionManager* colManager, Entity* tree_, float x, float y)
{
	//distribucion random de intervalo variable en distintas llamadas
	std::random_device seed;
	std::mt19937 gen{ seed() };
	std::uniform_int_distribution <>myrand(0, sdlutils().width());

	//crea entidad linterna
	auto lantern = mngr_->addEntity();

	mngr_->setHandler(ecs::_LANTERN, lantern);

	//atributos de linterna
	auto lanternAtribs = lantern->addComponent<BossAtributos>(30);
	auto lanternTr = lantern->addComponent<Transform>();
	auto lanternX = x;
	auto lanternY = y;

	//damos paramtros iniciales
	lanternTr->init(Vector2D(lanternX, lanternY), Vector2D(), 100, 100, 0.0f);
	//de momento con un color pocho
	lantern->addComponent<Image>(&sdlutils().images().at("lampara"));

	//collider de la lampara
	auto lanternCollider = lantern->addComponent <RectangleCollider>(lanternTr->getWidth(), lanternTr->getHeight());
	//se hace trigger
	lanternCollider->setIsTrigger(true);
	//le pasamos el collider al manager
	colManager->addCollider(lanternCollider);
	//la damos movimiento
	lantern->addComponent<LanternMovement>();

	lantern->addComponent<LanternCollisions>(colManager);
	lantern->addToGroup(ecs::_BOSSELEMENTS_GRP);
	auto lanternHPBar = mngr_->addEntity();
	lanternHPBar->addComponent<BossHPBar>();
	lanternHPBar->addToGroup(ecs::_UI_GRP);
}

void TreeScene::deactivateBoss()
{
	treeMngr_->deactivateBoss();
	playerCtrl_->setActive(false);
	playerDying = true;
}

bool TreeScene::getAble()
{
	return isAble;
}

void TreeScene::setAble(bool a)
{
	isAble = a;
}
