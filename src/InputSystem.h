#pragma once
#define _USE_MATH_DEFINES
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
	void InputToMovement();
private:
	bool gpArr[16];
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	int shoot = 0;
	int gpshoot = 0;
	const float sens = 0.5f; //controller joystick sensitivity
};