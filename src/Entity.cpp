#include "Entity.h"

void Entity::updateTransform() {

	switch (this->physType) {
	case PhysicsType::CAR:

		//position
		this->transform->pos.x = this->collisionBox->getGlobalPose().p.x;
		this->transform->pos.y = this->collisionBox->getGlobalPose().p.y;
		this->transform->pos.z = this->collisionBox->getGlobalPose().p.z;

		//rotation
		this->transform->rot.x = this->collisionBox->getGlobalPose().q.x;
		this->transform->rot.y = this->collisionBox->getGlobalPose().q.y;
		this->transform->rot.z = this->collisionBox->getGlobalPose().q.z;
		this->transform->rot.w = this->collisionBox->getGlobalPose().q.w;		

		break;
	case PhysicsType::STATIC:
		break;
	default:
		break;
	}
}

void Entity::CreateTransformFromPhysX(PxTransform physXTransform) {

	this->transform = new Transform();

	this->transform->pos.x = physXTransform.p.x;
	this->transform->pos.y = physXTransform.p.y;
	this->transform->pos.z = physXTransform.p.z;

	this->transform->rot.x = physXTransform.q.x;
	this->transform->rot.y = physXTransform.q.y;
	this->transform->rot.z = physXTransform.q.z;
	this->transform->rot.w = physXTransform.q.w;
}