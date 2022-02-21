#include "SimplePhysicsPlayer.h"
#include "../FrogBoss/BossAtributos.h"
#include "../../ecs/Entity.h"
#include "../Transform.h"


SimplePhysicsPlayer::SimplePhysicsPlayer(CollisionManager* colManager) : tr_(nullptr), colMan_(colManager), collider_(nullptr), invulnerable_(false), invTimer(0)
{
}

SimplePhysicsPlayer::~SimplePhysicsPlayer()
{
}

void SimplePhysicsPlayer::initComponent()
{
	tr_ = ent_->getComponent<Transform>();
	collider_ = ent_->getComponent<RectangleCollider>();
	attrib_ = ent_->getComponent<PlayerAttributes>();
	gravity_ = ent_->getComponent<SimpleGravity>();
	assert(tr_ != nullptr && collider_ != nullptr && attrib_ != nullptr);
}

void SimplePhysicsPlayer::update()
{
	//Gravedad
	if (!attrib_->isOnGround() && !gravity_->isActive()) gravity_->setActive(true);
	else if (gravity_->isActive()) gravity_->setActive(false);

	//Colisiones
	if (colMan_->hasCollisions(collider_)) {
	
		std::vector<RectangleCollider*> colliders = colMan_->getCollisions(collider_);
		
		for (auto c : colliders) {
		
			if (c->isActive() && !c->isTrigger()) {
			
				//colisiones
				auto pos = collider_->getPos(); //jugador
				auto posCollider = c->getPos(); //otros

				auto& velPlayer = tr_->getVel();

				if (pos.getX() < posCollider.getX()) {//colision por la izda
							
					velPlayer.setX(0);
					tr_->getPos().setX(c->getPos().getX() - collider_->getWidth());
					attrib_->setRightStop(true);
				}
				else if (pos.getX() > posCollider.getX() + c->getWidth()) {//colision por la derecha

					velPlayer.setX(0);
					attrib_->setLeftStop(true);
				}
				else { //dentro de la plataforma (eje x)
					if (pos.getY() <= posCollider.getY()) {//arriba
						velPlayer.setY(0);
						tr_->getPos().setY(c->getPos().getY() - collider_->getHeight());
						attrib_->setOnGround(true);

					}
					else {
						if (abs(velPlayer.getY()) > 5) {
							velPlayer.setY(0);
						}
					}
				}


				ecs::Entity* ent = c->getEntity();
				BossAtributos* bA = ent->getComponent<BossAtributos>();
				
				if (bA != nullptr) {
					
					if (!invulnerable_) {
						attrib_->damagePlayer(1);
						invulnerable_ = true;
						invTimer = sdlutils().currRealTime();
					}
										
				}
			}			
		}

		if (invTimer + 5000 > sdlutils().currRealTime()) return;
		invulnerable_ = false;
		
	}
}


