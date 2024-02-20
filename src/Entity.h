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
};