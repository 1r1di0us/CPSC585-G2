#pragma once

#include <string>
#include "Transform.h"
#include "Model.h"
#include "Car.h"

enum class PhysicsType {
	CAR,
	PROJECTILE,
	STATIC
};
	
class Entity {

public:
	std::string name;

	Model* model;
	Transform* transform;

	PhysicsType physType;
	PxRigidDynamic* collisionBox;

	//helper functions
	void updateTransform();

	//utility function to create an entities transform component given a physx transform
	void CreateTransformFromPhysX(PxTransform physXTransform);
};