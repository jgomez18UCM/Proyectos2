#include "HammerArm.h"
#include "../../../ecs/Entity.h"
#include "../../Transform.h"
#include "../../RectangleCollider.h"
#include "../../../sdlutils/SDLUtils.h"

HammerArm::HammerArm(bool leftHand) : leftHand_(leftHand), tr_(nullptr), state_(REPOSO), initialPos()
{
}

HammerArm::~HammerArm()
{
}

void HammerArm::initComponent()
{
	tr_ = ent_->getComponent<Transform>();
	collider_ = ent_->getComponent<RectangleCollider>();
	assert(tr_ != nullptr, collider_ != nullptr);

	initialPos = Vector2D(tr_->getPos().getX(), tr_->getPos().getY());
}

void HammerArm::goDiagonal()
{
	collider_->setActive(false);
	int xObjective;
	if (!leftHand_) xObjective = 0;
	else xObjective = sdlutils().width() - tr_->getWidth();
	if (abs(tr_->getPos().getX() - xObjective) > 5 || abs(tr_->getPos().getY() - sdlutils().height() + tr_->getHeight()) > 5) {
		Vector2D dir = Vector2D(xObjective - tr_->getPos().getX(), sdlutils().height() - tr_->getPos().getY() - tr_->getHeight());/*initialPos - tr_->getPos();*/
		tr_->getVel().set(dir.normalize() * handSpeed);
	}
	else {
		tr_->getVel().set(Vector2D(0, 0));
		tr_->getPos().set(Vector2D(xObjective, sdlutils().height() - tr_->getHeight()));
		changeState(CENTER);
	}
}

void HammerArm::goCenter()
{
	int objectivePos;

	collider_->setActive(true);
	collider_->setIsTrigger(true);


	if (leftHand_) {
		objectivePos = sdlutils().width() / 2;

		if (tr_->getPos().getX() > objectivePos) {
			tr_->getVel().set(Vector2D(-handSpeed * 2, 0));
		}
		else {
			tr_->getVel().set(Vector2D(0, 0));
			tr_->getPos().setX(objectivePos);
			changeState(BACK);
		}
	}
	else {
		objectivePos = sdlutils().width() / 2 - tr_->getWidth();

		if (tr_->getPos().getX() < objectivePos) {
			tr_->getVel().set(Vector2D(handSpeed * 2, 0));
		}
		else {
			tr_->getVel().set(Vector2D(0, 0));
			tr_->getPos().setX(objectivePos);
			lastTimeFloor = sdlutils().currRealTime();
			changeState(REPOSOSUELO);
		}
	}
}

void HammerArm::goBack()
{
	if (abs(tr_->getPos().getX() - initialPos.getX()) > 5 || abs(tr_->getPos().getY() - initialPos.getY()) > 5) {
		Vector2D dir = initialPos - tr_->getPos();
		tr_->getVel().set(dir.normalize() * handSpeed);
	}
	else {
		tr_->getVel().set(Vector2D(0, 0));
		tr_->getPos().set(initialPos);
		changeState(FIN);
	}
}

void HammerArm::stayFloor() {
	collider_->setIsTrigger(false);
	if (sdlutils().currRealTime() > lastTimeFloor + cooldoownInFloor)
		changeState(BACK);
}