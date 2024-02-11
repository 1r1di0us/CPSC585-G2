#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now
#include <math.h>
#include "PhysicsSystem.h"


class InputSystem {
public:
	InputSystem();
	void checkIfGamepadsPresent();
	void getGamePadInput();
	void getKeyboardInput(GLFWwindow* window);
	void InputToMovement(Entity* playerCar);
private:
	bool gpArr[16];
	//make these arrays at some point
	bool forward[17];
	bool backward[17];
	bool left[17];
	bool right[17];
	int shoot[17];
	const float sens = 0.5f; //controller joystick and trigger sensitivity
};