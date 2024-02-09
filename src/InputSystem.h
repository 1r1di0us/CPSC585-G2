#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now
#include <math.h>
#include "PhysicsSystem.h"

class InputSystem {
public:
	Entity* playerCar;
	InputSystem(Entity* pcar);
	void checkIfGamepadsPresent();
	void getGamePadInput();
	void getKeyboardInput(GLFWwindow* window);
private:
	bool gpArr[16];
	const float sens = 0.5f; //controller joystick sensitivity
};