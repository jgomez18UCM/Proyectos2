#include "Scene.h"

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
#include "../components/boss/BossComponents.h"
#include "../components/fondos/ParticleSystem.h"

#include "CollisionManager.h"

Scene::Scene() : mngr_(nullptr), camera_(), playerAttribs_()
{
}

Scene::~Scene()
{
	delete mngr_;
}

void Scene::init()
{
	mngr_ = new ecs::Manager();
#ifdef _DEBUG
	mngr_->setDebug(true);
#endif
	camera_ = mngr_->addEntity();
	auto camTr = camera_->addComponent<Transform>();
	mngr_->setHandler(ecs::_hdlr_CAMERA, camera_);
}

void Scene::update()
{
}

void Scene::backgroundmovement(std::string backgroundName)
{
	auto backgrs_ = mngr_->addEntity();
	auto backgrs_Tr = backgrs_->addComponent<Transform>(Vector2D(0, 0), Vector2D(), sdlutils().width(), sdlutils().height(), 0.0f);
	backgrs_->addComponent<FramedImage>(&sdlutils().images().at(backgroundName), 10, 8, 5000, 78, backgroundName);

	auto suelo = mngr_->addEntity();
	auto suelo_Tr = suelo->addComponent<Transform>(Vector2D(0, 0), Vector2D(), sdlutils().width(), sdlutils().height() / 5, 0.0f);
	suelo->addComponent<Image>(&sdlutils().images().at("Suelo"));
}

void Scene::background(std::string backgroundName, int height)
{
	auto backgr_ = mngr_->addEntity();
	auto backgr_Tr = backgr_->addComponent<Transform>(Vector2D(0, 0), Vector2D(), sdlutils().width(), sdlutils().height(), 0.0f);
	backgr_->addComponent<Image>(&sdlutils().images().at(backgroundName));

	auto suelo = mngr_->addEntity();

	if (backgroundName == "fondo2")
		auto suelo_Tr = suelo->addComponent<Transform>(Vector2D(0, sdlutils().height() - sdlutils().height() - 190), Vector2D(), sdlutils().width() + 20, sdlutils().height() + 190, 0.0f);
	else
		auto suelo_Tr = suelo->addComponent<Transform>(Vector2D(0, sdlutils().height() - height), Vector2D(), sdlutils().width(), height, 0.0f);


	suelo->addComponent<Image>(&sdlutils().images().at("Suelo"));
	backgr_->addToGroup(ecs::_BACKGROUND_1_GRP);
	suelo->addToGroup(ecs::_BACKGROUND_2_GRP);
}

void Scene::playerGenerator(CollisionManager* colManager, Entity* player_)
{
	playerAttribs_ = player_->addComponent<PlayerAttributes>();

	playerTr_ = player_->addComponent<Transform>();
	auto playerX = 0;
	auto playerY = sdlutils().height() / 2 - 25;
	playerTr_->init(Vector2D(playerX, playerY), Vector2D(), 50, 140, 0.0f, 0.5f);

	playerImg_ = player_->addComponent<FramedImage>(&sdlutils().images().at("Chica_Idle"), 5, 6, 5000, 30, "Chica_Idle");
	playerImg_->setAlpha(255);

	//IMPORTANTE: Ponerlo antes de CollideWithBorders siempre
	player_->addComponent<SimpleGravity>(1);
	//IMPORTANTE: Ponerlo antes del PlayerCtrl siempre porque si no se salta 2 veces
	bordersPlayer_ = player_->addComponent<CollideWithBorders>(100);
	bordersPlayer_->collisionx(true);

	//Se a�ade un collider al jugador
	auto playerCollider = player_->addComponent<RectangleCollider>(playerTr_->getWidth(), playerTr_->getHeight());
	colManager->addCollider(playerCollider);

	//IMPORTANTE :No poner estas f�sicas detr�s del playerctrl
	player_->addComponent<SimplePhysicsPlayer>(colManager);

	//Componente de ataque del jugador
	auto playerAttackCollider = player_->addComponent<Attack>(135, playerTr_->getHeight() * 1.8, -playerTr_->getHeight() * 1.5 / 3, colManager);
	colManager->addCollider(playerAttackCollider);
	playerAttackCollider->setIsTrigger(true);

	// float jumpForce, float speed, float deceleration, float rollSpeed
	playerCtrl_ = player_->addComponent<PlayerCtrl>(15, 6, 0.7, 10);
	mngr_->setHandler(ecs::_PLAYER, player_);

	auto playerLife_ = mngr_->addEntity();
	playerLife_->addComponent<PlayerUI>();
	playerLife_->addToGroup(ecs::_UI_GRP);

	player_->addToGroup(ecs::_PLAYER_GRP);
}
void Scene::playerGeneratorEscape(CollisionManager* colManager, Entity* player_)
{
	playerAttribs_ = player_->addComponent<PlayerAttributes>();

	playerTr_ = player_->addComponent<Transform>();
	auto playerX = 0;
	auto playerY = sdlutils().height() / 4;
	playerTr_->init(Vector2D(playerX, playerY), Vector2D(), 50, 140, 0.0f, 0.5f);

	playerImg_ = player_->addComponent<FramedImage>(&sdlutils().images().at("Chica_Idle"), 5, 6, 5000, 30, "Chica_Idle");
	playerImg_->setAlpha(255);

	//IMPORTANTE: Ponerlo antes de CollideWithBorders siempre
	player_->addComponent<SimpleGravity>(1);
	//IMPORTANTE: Ponerlo antes del PlayerCtrl siempre porque si no se salta 2 veces
	auto bordersPlayer_1 = player_->addComponent<CollideWithBorders>(100);
	bordersPlayer_1->collisionx(false);

	//Se a�ade un collider al jugador
	auto playerCollider = player_->addComponent<RectangleCollider>(playerTr_->getWidth(), playerTr_->getHeight());
	colManager->addCollider(playerCollider);

	//IMPORTANTE :No poner estas f�sicas detr�s del playerctrl
	player_->addComponent<SimplePhysicsPlayer>(colManager);

	//Componente de ataque del jugador
	auto playerAttackCollider = player_->addComponent<Attack>(135, playerTr_->getHeight() * 1.8, -playerTr_->getHeight() * 1.5 / 3, colManager);
	colManager->addCollider(playerAttackCollider);
	playerAttackCollider->setIsTrigger(true);

	// float jumpForce, float speed, float deceleration, float rollSpeed
	playerCtrl_ = player_->addComponent<PlayerCtrl>(15, 6, 0.7, 10);
	mngr_->setHandler(ecs::_PLAYER, player_);

	auto playerLife_ = mngr_->addEntity();
	playerLife_->addComponent<PlayerUI>();
	playerLife_->addToGroup(ecs::_UI_GRP);

	player_->addToGroup(ecs::_PLAYER_GRP);
}

void Scene::checkPlayerFinishedDying()
{
	if (playerDeathTimer_->currTime() >= PLAYER_DEATH_DELAY) 
	{
		playerAttribs_->setDefeated(true);
		playerAttribs_->deactivateStop();
	}
}

void Scene::checkPlayerDied()
{
	if (playerAttribs_->getLives() <= 0) 
	{
		playerImg_->setAlpha(0);

		ParticleSystem* particlesys = new ParticleSystem(&sdlutils().images().at("you_died"), mngr_);
		particlesys->createParticlesBossSignYouDied(1);

		ParticleSystem* particlesys2 = new ParticleSystem(&sdlutils().images().at("particula_damage"), mngr_);
		particlesys2->createParticlesKillPlayer(50, playerTr_->getPos().getX() + (playerTr_->getWidth() / 2), playerTr_->getPos().getY() + (playerTr_->getHeight() / 2));
		ParticleSystem* particlesys3 = new ParticleSystem(&sdlutils().images().at("particula_negro"), mngr_);
		particlesys3->createParticlesKillPlayer(50, playerTr_->getPos().getX() + (playerTr_->getWidth() / 2), playerTr_->getPos().getY() + (playerTr_->getHeight() / 2));

		SoundEffect* s = &sdlutils().soundEffects().at("sfx_player_dead");
		s->play();

		deactivateBoss();
		playerDeathTimer_->reset();
	}
}
