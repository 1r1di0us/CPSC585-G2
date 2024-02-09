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

	Car* car;
};