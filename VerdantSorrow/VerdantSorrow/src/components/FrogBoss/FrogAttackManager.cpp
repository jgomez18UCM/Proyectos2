#include "FrogAttackManager.h"
#include "../../ecs/Entity.h"
#include "../../ecs/Manager.h"
#include "FrogBigJump.h"
#include "FrogJump.h"
#include "TongueAttack.h"
#include "../Transform.h"
#include "../wave/WaveMovement.h"
#include "../RectangleCollider.h"
#include "../RectangleRenderer.h"
#include "../../game/CollisionManager.h"
#include "BossAtributos.h"
#include "../../sdlutils/SDLUtils.h"

FrogAttackManager::FrogAttackManager() : frogJump_(), bigJump_(), fly_(), player_(), tr_(),
				frogState_(FIRST_PHASE), jumping_(), jumpDirection_(1), jumpsUntilNextTongue_(0), flySpacing_(0), collManager_(), tongueAttack_()
{
}

FrogAttackManager::FrogAttackManager(CollisionManager* collManager) : frogJump_(), bigJump_(), fly_(), player_(), tr_(), collManager_(collManager),
					frogState_(FIRST_PHASE), jumping_(), jumpDirection_(1), jumpsUntilNextTongue_(0), flySpacing_(0), tongueAttack_()
{
}

FrogAttackManager::~FrogAttackManager()
{
}

void FrogAttackManager::initComponent()
{
	frogJump_ = ent_->addComponent<FrogJump>(30);
	bigJump_ = ent_->addComponent<FrogBigJump>(40);
	tongueAttack_ = ent_->addComponent<TongueAttack>();
	tr_ = ent_->getComponent<Transform>();
	player_ = mngr_->getHandler(ecs::_PLAYER)->getComponent<Transform>();
	bool correct = tr_ != nullptr && frogJump_ != nullptr && tongueAttack_ != nullptr && bigJump_ != nullptr && player_ != nullptr;	
	assert(correct);
}

void FrogAttackManager::update()
{
	auto& rand = sdlutils().rand();
	if (frogState_ == FIRST_PHASE) {
		if (jumpsUntilNextTongue_ == 0 && !jumping_) {
			//TODO spawn fly, tongue attack
			std::cout << "Lenguetazo" << std::endl;
			jumpsUntilNextTongue_ = rand.nextInt(3, 5);
		}
		if (jumping_ && tr_->getPos().getY() + tr_->getHeight() > sdlutils().height()) {
			jumping_ = false;
			--jumpsUntilNextTongue_;
		}
		if (!jumping_) {
			int dir = 1;
			frogJump_->attack(dir);
		}
	}else if (FLY_DIED) {
		//Cambio a sprite enfadado
		bigJump_->attack(0);
	}
	else if (SECOND_PHASE) {
		auto jump = rand.nextInt(0, 100);
	}

}

ecs::Entity* FrogAttackManager::createFly()
{
	fly_ = mngr_->addEntity();
	auto attr = fly_->addComponent<Transform>();
	attr->init(Vector2D(player_->getPos().getX(), player_->getPos().getY()), Vector2D(), 100, 100, 0.0f);
	auto coll = fly_->addComponent<RectangleCollider>(tr_->getWidth(), tr_->getHeight());
	coll->setIsTrigger(true);
	collManager_->addCollider(coll);
	fly_->addComponent<RectangleRenderer>();
	return fly_;;
}

void  FrogAttackManager::createWave(int dir)
{
	//Se crea la onda expansiva
	auto Wave = mngr_->addEntity();
	//Se a�ade el transform
	auto WaveTr = Wave->addComponent<Transform>();
	auto WaveX = tr_->getPos().getX();
	auto WaveY = sdlutils().height() - 50;
	//dir = {-1, 1}
	auto WaveDir = dir;
	auto WaveSpeed = 5;
	//Se le dan las posiciones iniciales, velocidad, ancho y alto a la onda
	WaveTr->init(Vector2D(WaveX, WaveY), Vector2D(), 150, 50, 0.0f);
	//Se le a�ade un color inicial a la onda
	Wave->addComponent<RectangleRenderer>(SDL_Color());

	//Se a�ade un collider a la onda
	auto waveCollider = Wave->addComponent<RectangleCollider>(WaveTr->getWidth(), WaveTr->getHeight());
	waveCollider->setIsTrigger(true);
	//Se a�ade el collider al colliderGameManager
	collManager_->addCollider(waveCollider);
	//Se a�ade el movimiento horizontal
	Wave->addComponent<WaveMovement>(WaveDir, WaveSpeed);

}

void FrogAttackManager::onFlyDied() {
	frogState_ = FLY_DIED;
}
