#include "Punietazo.h"
#include "../../../sdlutils/SDLUtils.h"
#include "HandsManager.h"
#include "../../../ecs/Entity.h"
#include "../../../ecs/Manager.h"
#include "../../../sdlutils/SDLUtils.h"
#include "../../Transform.h"
#include "../../RectangleCollider.h"



Punietazo::Punietazo() :hitTime_(), goBackTime_(), state_(REPOSO), handSpeed_(6), dW(4), dH(4)
{

}

Punietazo::~Punietazo()
{
}

void Punietazo::initComponent()
{
	tr_ = ent_->getComponent<Transform>();
	assert(tr_ != nullptr);
	col_ = ent_->getComponent<RectangleCollider>();
	assert(col_ != nullptr);
	playertr_ = mngr_->getHandler(ecs::_PLAYER)->getComponent<Transform>();
	assert(playertr_ != nullptr);

	initialwidth_ = tr_->getWidth();
	initialheight_ = tr_->getHeight();
	initialpos_ = tr_->getPos();

	maxWidth_ = 2 * initialwidth_;
	maxHeight_ = 2 * initialheight_;
}

void Punietazo::goDown()
{
	int objectivePos = sdlutils().height() - tr_->getHeight();

	if (tr_->getPos().getY() < objectivePos) {

		tr_->getVel().setY(handSpeed_);
		col_->setActive(false);
	}
	else {
		tr_->getPos().setY(objectivePos);
		changeState(FOLLOW);
	}
}

void Punietazo::followPlayer()
{
	auto handPos = tr_->getPos();
	auto playerPos = playertr_->getPos();

	if (!(handPos.getX() >= playerPos.getX() && handPos.getX() < playerPos.getX() + playertr_->getWidth())) {

		tr_->getVel() = (handPos.getX() > playerPos.getX() ? Vector2D(-handSpeed_, 0) : Vector2D(handSpeed_, 0));
	}
	else
	{
		hitTime_ = sdlutils().currRealTime();
		changeState(HIT);
	}
}

void Punietazo::hit()
{
	auto width = tr_->getWidth();
	auto height = tr_->getHeight();
	tr_->getVel().set(Vector2D(0, 0));

	if (sdlutils().currRealTime() - hitTime_ >= 350) {
		col_->setIsTrigger(true);
		col_->setActive(true);

		col_->setWidth(width);
		col_->setHeight(height);

		changeState(BACK);

		goBackTime_ = sdlutils().currRealTime();
	}
	else {
		int nextWidth = std::min(width + dW, maxWidth_);
		int nextHeight = std::min(height + dH, maxHeight_);

		tr_->getPos().setY(tr_->getPos().getY() - (nextWidth - width));
		tr_->getPos().setX(tr_->getPos().getX() - (nextHeight - height));

		tr_->setWidth(nextWidth);
		tr_->setHeight(nextHeight);
	}
}

void Punietazo::goBack()
{
	auto width = tr_->getWidth();
	auto height = tr_->getHeight();

	if (sdlutils().currRealTime() - goBackTime_ >= 750) {

		col_->setActive(false);

		bool isSize = false, isPos = false;

		if (abs(tr_->getPos().getX() - initialpos_.getX()) > 5 || abs(tr_->getPos().getY() - initialpos_.getY()) > 5) {
			Vector2D dir = initialpos_ - tr_->getPos();
			tr_->getVel().set(dir.normalize() * handSpeed_);
		}
		else { //correct pos
			tr_->getVel().set(Vector2D(0, 0));
			tr_->getPos().set(initialpos_);
			isPos = true;
		}


		if (abs(width - initialwidth_) > 1 || abs(height - initialheight_) > 1) {

			int nextWidth = std::max(width - dW, initialwidth_);
			int nextHeight = std::max(height - dH, initialheight_);

			tr_->getPos().setY(tr_->getPos().getY() + (nextWidth - width));
			tr_->getPos().setX(tr_->getPos().getX() + (nextHeight - height));

			tr_->setWidth(nextWidth);
			tr_->setHeight(nextHeight);
		}
		else { //correct size
			tr_->setWidth(initialwidth_);
			tr_->setHeight(initialheight_);
			isSize = true;
		}

		if (isPos && isSize) {
			changeState(FIN);

			col_->setWidth(width);
			col_->setHeight(height);
			col_->setActive(true);
			col_->setIsTrigger(false);
		}

	}

}



