#include "FrogAttackManager.h"
#include "../../../ecs/Entity.h"
#include "../../../ecs/Manager.h"
#include "FrogBigJump.h"
#include "FrogJump.h"
#include "TongueAttack.h"
#include "../../Transform.h"
#include "../wave/WaveMovement.h"
#include "../../RectangleCollider.h"
#include "../../RectangleRenderer.h"
#include "../../../game/CollisionManager.h"
#include "../BossAtributos.h"
#include "../../../sdlutils/SDLUtils.h"
#include "../../FramedImage.h"
#include "FlyHp.h"

FrogAttackManager::FrogAttackManager() : frogJump_(), bigJump_(), fly_(), player_(), tr_(),
				frogState_(FLY_DIED), jumping_(false), jumpingBig_(false), jumpDirection_(-1), 
				jumpsUntilNextTongue_(0), flySpacing_(0), collManager_(), tongueAttack_(), 
				tongueDelay_(3000), animState_(ANIM_IDLE), animNewState_(ANIM_IDLE)
{
}

FrogAttackManager::FrogAttackManager(CollisionManager* collManager) : frogJump_(), bigJump_(), 
				fly_(), player_(), tr_(), collManager_(collManager), frogState_(FLY_DIED),
				jumping_(false), jumpingBig_(false), jumpDirection_(1), jumpsUntilNextTongue_(0), 
				flySpacing_(0), tongueAttack_(), tongueDelay_(3000), animState_(ANIM_IDLE), 
				animNewState_(ANIM_IDLE)
{
}

FrogAttackManager::~FrogAttackManager()
{
}

void FrogAttackManager::initComponent()
{
	frogJump_ = ent_->addComponent<FrogJump>(30);
	bigJump_ = ent_->addComponent<FrogBigJump>(40);
	tongueAttack_ = ent_->addComponent<TongueAttack>(collManager_);
	tr_ = ent_->getComponent<Transform>();
	player_ = mngr_->getHandler(ecs::_PLAYER)->getComponent<Transform>();
	attr_ = ent_->getComponent<BossAtributos>();
	anim_ = ent_->getComponent<FramedImage>();

	musicaFase2_ = &sdlutils().musics().at("musica_rana_fase2");
	musicaFase2_->play();
	musicaFase2_->setMusicVolume(0);

	musicaFase1_ = &sdlutils().soundEffects().at("musica_rana_fase1");
	musicaFase1_->play(10, 0);
	musicaFase1_->setChannelVolume(80, 0);

	bool correct = tr_ != nullptr && frogJump_ != nullptr && tongueAttack_ != nullptr && bigJump_ != nullptr && player_ != nullptr;	
	assert(correct);
}

//Patrones de ataque de la rana
void FrogAttackManager::update()
{
	auto& rand = sdlutils().rand();
	if (attr_->getLife() <= 0) {
		std::cout << "Muerte" << std::endl;
		animNewState_ = ANIM_DEATH;
		if(anim_->getFrameNum()==16)
			ent_->setAlive(false);
		return;
	}
	flipOnBorders();
	switch (frogState_) {
		case JUMPING:
			if (attr_->isOnGround()) {
				onGrounded(jumping_, false);
			}
			break;
		case JUMPING_BIG:
			if (attr_->isOnGround()) {
				onGrounded(jumpingBig_, true);
			}
			break;
		case TONGUE:
			if (tongueAttack_->finished()) 
			{
				frogState_ = WAITING;
				
				delay_ = rand.nextInt(1000, 3000);
				lastUpdate_ = sdlutils().currRealTime();
			}
			
			break;
		case CALC_NEXT_ATTACK:
			nextAttack();
			break;
		case WAITING:
			//std::cout << "esperando" << std::endl;
			if (delay_ + lastUpdate_ < sdlutils().currRealTime()) {
				frogState_ = CALC_NEXT_ATTACK;
			}
			break; 
		case WAITING_FOR_TONGUE:
			//std::cout << "esperando" << std::endl;
			if (lastUpdate_ + tongueDelay_ < sdlutils().currRealTime()) {
				tongueAttack_->attack(!secondPhase_);
				frogState_ = TONGUE;
			}
			break;
		case FLY_DIED:
			if (!jumping_ && !jumpingBig_) {
				bigJump_->attack(0);
				tongueAttack_->cancel();
				jumpingBig_ = true;
				frogState_ = JUMPING_BIG;
				angry_ = true;
				//Lanzar animacion de salto largo
				animNewState_ = ANIM_BIG_JUMP;
			}
			else
			{
				if (jumping_) frogState_ = JUMPING;
				else if (jumpingBig_) frogState_ = JUMPING_BIG;
			}
			break;
		default:
			break;
	}if (animState_ != animNewState_) {
		animState_ = animNewState_;
		switch (animState_)
		{
		case FrogAttackManager::ANIM_IDLE:
			if (!secondPhase_)anim_->changeanim(&sdlutils().images().at("rana_idle"), 4, 6, (1000 / 30) * 24, 24, "rana_idle");
			else anim_->changeanim(&sdlutils().images().at("rana_enfadada_idle"), 4, 6, (1000 / 30) * 24, 24, "rana_enfadada_idle");
			break;
		case FrogAttackManager::ANIM_JUMP:
			if (!secondPhase_) anim_->changeanim(&sdlutils().images().at("rana_jump"), 6, 6, (1000 / 30) * 32, 32, "rana_jump");
			else anim_->changeanim(&sdlutils().images().at("rana_enfadada_jump"), 6, 6, (1000 / 30) * 32, 32, "rana_enfadada_jump");
			if (anim_->getFrameNum() == 32) animNewState_ = ANIM_IDLE;
			break;
		case FrogAttackManager::ANIM_BIG_JUMP:
			if (!secondPhase_) {
				anim_->changeanim(&sdlutils().images().at("rana_jump"), 6, 6, (1000 / 30) * 32, 32, "rana_jump");
				if (anim_->getFrameNum() == 32) animNewState_ = ANIM_JUMP_TO_VULNERABLE;
			}
			else {
				anim_->changeanim(&sdlutils().images().at("rana_enfadada_jump"), 6, 6, (1000 / 30) * 32, 32, "rana_enfadada_jump");
				if (anim_->getFrameNum() == 32) animNewState_ = ANIM_IDLE;
			}

			break;
		case FrogAttackManager::ANIM_TONGUE:
			if (!secondPhase_)anim_->changeanim(&sdlutils().images().at("rana_lengua"), 4, 6, (1000 / 30) * 24, 24, "rana_lengua");
			else anim_->changeanim(&sdlutils().images().at("rana_enfadada_lengua"), 4, 6, (1000 / 30) * 24, 24, "rana__enfadada_lengua");
			if (anim_->getFrameNum() == 24) anim_->select_sprite(6, 4);
			break;
		case FrogAttackManager::ANIM_CHANGE_PHASE:
			anim_->changeanim(&sdlutils().images().at("rana_cambio_fase"), 4, 6, (1000 / 30) * 23, 23, "rana_cambio_fase");
			if (anim_->getFrameNum() == 23) animNewState_ = ANIM_IDLE;
			break;
		case FrogAttackManager::ANIM_JUMP_TO_VULNERABLE:
			anim_->changeanim(&sdlutils().images().at("rana_salto_a_vulnerable"), 2, 6, (1000 / 30) * 10, 10, "rana_salto_a_vulnerable");
			if (anim_->getFrameNum() == 10) animNewState_ = ANIM_VULNERABLE;
			break;
		case FrogAttackManager::ANIM_VULNERABLE:
			anim_->changeanim(&sdlutils().images().at("rana_vulnerable"), 4, 6, (1000 / 30) * 21, 21, "rana_vulnerable");
			if (anim_->getFrameNum() == 10) animNewState_ = ANIM_VULNERABLE_TO_IDLE;
			break;
		case FrogAttackManager::ANIM_VULNERABLE_TO_IDLE:
			anim_->changeanim(&sdlutils().images().at("rana_vulnerable_a_idle"), 1, 5, (1000 / 30) * 5, 5, "rana_vulnerable_a_idle");
			if (anim_->getFrameNum() == 5) animNewState_ = ANIM_IDLE;
			break;
		case FrogAttackManager::ANIM_DEATH:
			anim_->changeanim(&sdlutils().images().at("rana_enfadada_muerte"), 3, 6, (1000 / 30) * 16, 16, "rana_enfadada_muerte");
			if (anim_->getFrameNum() == 14) anim_->select_sprite(4, 3);
			break;
		default:
			break;
		}
	}
}



ecs::Entity* FrogAttackManager::createFly()
{
	fly_ = mngr_->addEntity();
	auto fTr = fly_->addComponent<Transform>();
	auto flyY = sdlutils().height() - tr_->getHeight();
	auto flyX = player_->getPos().getX();
	fTr->init(Vector2D(flyX, flyY), Vector2D(), 100, 100, 0.0f);
	auto coll = fly_->addComponent<RectangleCollider>(fTr->getWidth(), fTr->getHeight());
	coll->setIsTrigger(true);
	collManager_->addCollider(coll);
	fly_->addComponent<FramedImage>(&sdlutils().images().at("mosca"), 6, 6, 2000, 31, "mosca");
	fly_->addComponent<FlyHp>(this);
	mngr_->setHandler(ecs::_FLY, fly_);
	return fly_;
}

void  FrogAttackManager::createWave(int dir)
{
	//Se crea la onda expansiva
	auto Wave = mngr_->addEntity();
	//Se añade el transform
	auto WaveTr = Wave->addComponent<Transform>();
	auto WaveX = tr_->getPos().getX();
	auto WaveY = sdlutils().height() - 50;
	//dir = {-1, 1}
	auto WaveDir = dir;
	if (WaveDir == 1) {
		WaveX += tr_->getWidth();
	}
	auto WaveSpeed = 5;
	//Se le dan las posiciones iniciales, velocidad, ancho y alto a la onda
	WaveTr->init(Vector2D(WaveX, WaveY), Vector2D(), 150, 50, 0.0f);
	//Se le añade un color inicial a la onda
	Wave->addComponent<RectangleRenderer>(SDL_Color());

	//Se a�ade un collider a la onda
	auto waveCollider = Wave->addComponent<RectangleCollider>(WaveTr->getWidth(), WaveTr->getHeight());
	waveCollider->setIsTrigger(true);
	//Se a�ade el collider al colliderGameManager
	collManager_->addCollider(waveCollider);
	//Se a�ade el movimiento horizontal
	Wave->addComponent<WaveMovement>(WaveDir, WaveSpeed);

}

void FrogAttackManager::createWaves()
{
	createWave(1);
	createWave(-1);
}

void FrogAttackManager::onFlyDied() {
	frogState_ = FLY_DIED;
}

void FrogAttackManager::flipOnBorders()
{
	if (jumpDirection_ == 1 && attr_->isOnRightBorder()) {
		anim_->flipX(false);
		jumpDirection_ = -1;
	}
	else if (jumpDirection_ == -1 && attr_->isOnLeftBorder()) {
		jumpDirection_ = 1;
		anim_->flipX(true);
	}
}

void FrogAttackManager::onGrounded(bool& jump, bool isBig)
{
	frogState_ = WAITING;
	jump = false;
	delay_ = sdlutils().rand().nextInt(1000, 2500);
	lastUpdate_ = sdlutils().currRealTime();
	if (isBig) {
		if (secondPhase_) jumpsUntilNextTongue_--;
		createWaves();
		if (angry_) /*Volver a rana normal*/;
	}
	else {
		jumpsUntilNextTongue_--;
	}
	//Iniciar animaci�n Idle y ponerla en repeticii�n, si esta en segunda fase poner idle de segunda fase -> !MIRIAM EL REPEAT DE FRAMED IMAGE ESTA INVERTIDO!
	animNewState_ = ANIM_IDLE;
}

void FrogAttackManager::nextAttack()
{
	if(!secondPhase_ && attr_->getLife() <= attr_->getMaxHp() * 0.5) {
		//Lanzar animacion del cambio de fase
		animNewState_ = ANIM_CHANGE_PHASE;
		SoundEffect* s = &sdlutils().soundEffects().at("sfx_cambio_fase");
		s->play();
		musicaFase2_->setMusicVolume(100);
		musicaFase1_->pauseChannel(0);
		frogState_ = WAITING;
		//delay_  = duracion de la animacion de cambio de fase
		lastUpdate_ = sdlutils().currRealTime();
		secondPhase_ = true;
		return;
	}
	if (jumpsUntilNextTongue_ == 0) {
		jumpsUntilNextTongue_ = sdlutils().rand().nextInt(3, 5);
		frogState_ = TONGUE;
		//No tengo ni idea de como se lanzara la animacion aqui
		if (!secondPhase_) {
			createFly();
			animNewState_ = ANIM_TONGUE;
		}
		frogState_ = WAITING_FOR_TONGUE;
	}
	else {
		int nextJump = secondPhase_ ? sdlutils().rand().nextInt(0, 100) : 100;
		if (nextJump >= 30) {
			//lanzar animacion de salto en el metodo attack
			animNewState_ = ANIM_JUMP;
			frogJump_->attack(jumpDirection_);
			frogState_ = JUMPING;
		}
		else {
			bigJump_->attack(jumpDirection_);
			//lanzar animavion de salto en el metodo attack
			frogState_ = JUMPING_BIG;
			animNewState_ = ANIM_BIG_JUMP;
		}

	}
}

