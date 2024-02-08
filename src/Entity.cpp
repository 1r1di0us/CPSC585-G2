#include "Entity.h"

void Entity::updateTransform() {

	switch (this->physType) {
	case PhysicsType::CAR:

		//position
		this->transform->pos.x = this->car->carTransform.p.x;
		this->transform->pos.y = this->car->carTransform.p.y;
		this->transform->pos.z = this->car->carTransform.p.z;

		//rotation
		this->transform->rot.x = this->car->carTransform.q.x;
		this->transform->rot.y = this->car->carTransform.q.y;
		this->transform->rot.z = this->car->carTransform.q.z;
		this->transform->rot.w = this->car->carTransform.q.w;

		break;
	case PhysicsType::PROJECTILE:

		//position
		this->transform->pos.x = this->projectile->body->getGlobalPose().p.x;
		this->transform->pos.y = this->projectile->body->getGlobalPose().p.y;
		this->transform->pos.z = this->projectile->body->getGlobalPose().p.z;

		//rotation
		this->transform->rot.x = this->projectile->body->getGlobalPose().q.x;
		this->transform->rot.y = this->projectile->body->getGlobalPose().q.y;
		this->transform->rot.z = this->projectile->body->getGlobalPose().q.z;
		this->transform->rot.w = this->projectile->body->getGlobalPose().q.w;

		break;
	case PhysicsType::STATIC:
		break;
	default:
		break;
	}
}