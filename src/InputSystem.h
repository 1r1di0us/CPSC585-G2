#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now

class InputSystem {
public:
	void update();
private:
	std::vector<Entity> entitiesWithInput;
};