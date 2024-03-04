#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now
#include <math.h>
#include "PhysicsSystem.h"
#include "SharedDataSystem.h"
#include <chrono>

class InputSystem {
public:

	SharedDataSystem* dataSys;

	InputSystem(SharedDataSystem* dataSys);
	void checkIfGamepadsPresent();
	void getGamePadInput();
	void getKeyboardInput(GLFWwindow* window);
	bool InputToMovement(std::chrono::duration<double> deltaTime);
	void InputToMenu();

private:
	double brakeTimer;
	bool gpArr[16];
	//make these arrays at some point
	bool forward[17];
	bool backward[17];
	bool left[17];
	bool right[17];
	bool confirm[17];
	int shoot[17];
	bool reverse[17];
	const float sens = 0.5f; //controller joystick and trigger sensitivity
	const float gasPedal = 1;
	bool menuLeftPressed = false;
	bool menuRightPressed = false;
};