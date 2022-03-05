#include "RectangleCollider.h"
#include "../ecs/Entity.h"
#include "Transform.h"
#include "../sdlutils/SDLUtils.h"


RectangleCollider::RectangleCollider() : tr_(nullptr), height_(), width_(), pos_(), isTrigger_(false), active_(true)
{
}
RectangleCollider::RectangleCollider(float width, float height): tr_(nullptr), height_(height), width_(width), pos_(), isTrigger_(false), active_(true)
{
}

RectangleCollider::~RectangleCollider()
{
}

void RectangleCollider::initComponent()
{
	tr_ = ent_->getComponent<Transform>();
	assert(tr_ != nullptr);
}

void RectangleCollider::update()
{
	Vector2D contPos = tr_->getPos();

	pos_ = Vector2D(contPos.getX() + (tr_->getWidth() - width_)/2, contPos.getY() + (tr_->getHeight() - height_)/2);
}


void RectangleCollider::debug()
{
	if (isActive())
		drawCollider();
}


SDL_Rect RectangleCollider::getCollider()
{
	return {(int)pos_.getX(), (int)pos_.getY(), (int)width_, (int)height_};
}

void RectangleCollider::setIsTrigger(bool set)
{
	isTrigger_ = set;
}

bool RectangleCollider::isTrigger()
{
	return isTrigger_;
}

void RectangleCollider::setActive(bool set)
{
	active_ = set;
}

bool RectangleCollider::isActive()
{
	return active_;
}

void RectangleCollider::drawCollider()
{
	SDL_Rect r = getCollider();
	SDL_SetRenderDrawColor(sdlutils().renderer(), 255,0,0, 255);
	SDL_RenderDrawRect(sdlutils().renderer(),&r);
}

Entity* RectangleCollider::getEntity() {
	return ent_;
}
