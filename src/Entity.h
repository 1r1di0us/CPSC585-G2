#pragma once

#include <string>
#include "Transform.h"
#include "Model.h"
#include "Car.h"
#include "Projectile.h"

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

	Car* car;
	Projectile* projectile;
};