#pragma once

#include <string>
#include "Transform.h"
#include "Model.h"
#include "Car.h"

enum class PhysicsType {
	CAR,
	STATIC
};
	
class Entity {

public:
	std::string name;
	Model* model;
	Transform* transform;
	PhysicsType physType;

	PxRigidDynamic* collisionBox;

	Car* car;
	//cant lie i kind of hate this, just tickles me wrong for some reason. it works tho - david
	std::vector<Transform*> projectileTransformList;

	//helper functions
	void updateTransform();

	//utility function to convert a physx transfom to glm
		//USELESS HERE, NEEDS TO BE MOVED INTO HELPER CLASS OR SOMETHING
	//temporary
	void CreateTransformFromPhysX(PxTransform physXTransform) {

		this->transform = new Transform();

		this->transform->pos.x = physXTransform.p.x;
		this->transform->pos.y = physXTransform.p.y;
		this->transform->pos.z = physXTransform.p.z;

		this->transform->rot.x = physXTransform.q.x;
		this->transform->rot.y = physXTransform.q.y;
		this->transform->rot.z = physXTransform.q.z;
		this->transform->rot.w = physXTransform.q.w;
	}
};