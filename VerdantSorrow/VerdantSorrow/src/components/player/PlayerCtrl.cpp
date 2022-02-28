#include "PlayerCtrl.h"
#include "../../sdlutils/InputHandler.h"
#include "../../ecs/Entity.h"
#include "../Transform.h"
#include "Attack.h"
#include "../../sdlutils/SDLUtils.h"

using namespace std;
PlayerCtrl::~PlayerCtrl()
{
}

void PlayerCtrl::update()
{
	auto& ihdlr = ih();
	auto currentTime = sdlutils().currRealTime();
	auto& vel = tr_->getVel();
	bool isAttacking = ent_->getComponent<Attack>()->isActive();

	//Si ha pasado el tiempo actual es mayor que cuando se activ� el roll + su duraci�n
	//Se desactiva y se activa el deslizar
	if (currentTime >= lastRoll + rollDuration && isRolling) {
		deslizar = true;
		isRolling = false;
	}

	//el jugador solo se puede mover si no esta atacando ni haciendo voltereta
	if (!isAttacking && !isRolling && ihdlr.keyDownEvent()) {
		//salto
		if ((ihdlr.isKeyDown(SDLK_w) || ihdlr.isKeyDown(SDLK_SPACE)) && attrib_->isOnGround()) {
			
			vel.set(Vector2D(vel.getX(), -jumpForce_));
			attrib_->setOnGround(false);
			deslizar = false;
		}
		//movimiento izquierda
		if (ihdlr.isKeyDown(SDLK_a) && !attrib_->isLeftStop()) {
			 
			vel.set(Vector2D(-speed_, vel.getY()));
			movementDir_ = -1;
			deslizar = false;
		}
		//movimiento derecha
		else if (ihdlr.isKeyDown(SDLK_d) && !attrib_->isRightStop()) {

			vel.set(Vector2D(speed_, vel.getY()));
			movementDir_ = 1;
			deslizar = false;
		}

		//Roll
		//Si ha pasado el tiempo suficiente como para volver a hacer roll y se pulsa el shift
		//Si es movementdir = -1(izquierda) se da velocidad hacia la izquierda,
		//Si es movementdir = 1(derecha)se da velocidad hacia la derecha
		//Se activa el cooldown y el booleano que informa que est� haciendo el roll
		if (ihdlr.isKeyDown(SDLK_LSHIFT)){
			if (currentTime >= lastRoll + rollDuration + rollCooldown) {
				if (movementDir_ < 0) vel.set(Vector2D(-rollSpeed_, vel.getY()));
				else vel.set(Vector2D(rollSpeed_, vel.getY()));
				lastRoll = currentTime;
				isRolling = true;
			}
		}
	}
	else if (ihdlr.isKeyUp(SDLK_a) && ihdlr.isKeyUp(SDLK_d) && ihdlr.isKeyUp(SDLK_w) && !isRolling && !isAttacking) {
		cout << "deslizo" << endl;
		deslizar = true;
	}
		
	//Si deslizar est� activado, es decir ha dejado de pulsar d y a
	//Si la velocidad es mayor que 1, positiva o negativa se ir� reduciendo poco a poco
	if ((tr_->getVel().getX() >= 1 && movementDir_ > 0) || (tr_->getVel().getX() <= -1 && movementDir_ < 0)){
		if (deslizar)
			vel.set(Vector2D(tr_->getVel().getX() * deceleration_, vel.getY()));
	}
	//Al llegar a menor de 1 se pondr� a 0 directamente y se desactivar� deslizar
	else if (((tr_->getVel().getX() < 1 && movementDir_ > 0) || (tr_->getVel().getX() > -1 && movementDir_ < 0))
		&& deslizar && attrib_->isOnGround()) {

		vel.set(Vector2D(0, vel.getY()));
		deslizar = false;
	}

	//Da igual lo que pase si ataca, que va a pararse en seco
	if (isAttacking){
		vel.set(Vector2D(0, vel.getY()));
		deslizar = false;
	}
}

void PlayerCtrl::initComponent()
{
	tr_ = ent_->getComponent<Transform>();
	assert(tr_ != nullptr);
	
	attrib_ = ent_->getComponent<PlayerAttributes>();
	assert(attrib_ != nullptr);
	playerCol_ = ent_->getComponent<RectangleCollider>();
	assert(playerCol_ != nullptr);
}

// Realiza un knockback en la direccion especificada
void PlayerCtrl::doKnockback(int dir) {

	auto& vel = tr_->getVel();
	vel.set(Vector2D(knockbackForceX * dir, -knockbackForceY));
	attrib_->setOnGround(false);

	deslizar = true;
}
