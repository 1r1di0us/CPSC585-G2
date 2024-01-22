#pragma once

#include <string>
#include "Transform.h"
#include "Model.h"

class Entity {

public:
	std::string name;
	Model* model;
	Transform* transform;
};