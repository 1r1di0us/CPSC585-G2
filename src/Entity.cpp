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

		//updating the list of projectiles car has fired
		for (int i = 0; i < this->projectileTransformList.size(); i++) {

			//position
			this->projectileTransformList[i]->pos.x = this->car->projectileBodyList[i]->getGlobalPose().p.x;
			this->projectileTransformList[i]->pos.y = this->car->projectileBodyList[i]->getGlobalPose().p.y;
			this->projectileTransformList[i]->pos.z = this->car->projectileBodyList[i]->getGlobalPose().p.z;

			//rotation
			this->projectileTransformList[i]->rot.x = this->car->projectileBodyList[i]->getGlobalPose().q.x;
			this->projectileTransformList[i]->rot.y = this->car->projectileBodyList[i]->getGlobalPose().q.y;
			this->projectileTransformList[i]->rot.z = this->car->projectileBodyList[i]->getGlobalPose().q.z;
			this->projectileTransformList[i]->rot.w = this->car->projectileBodyList[i]->getGlobalPose().q.w;
		}

		break;
	case PhysicsType::STATIC:
		break;
	default:
		break;
	}
}