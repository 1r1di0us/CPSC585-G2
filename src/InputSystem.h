#pragma once
#define _USE_MATH_DEFINES
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now
#include <math.h>
#include <chrono>
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
	int InputToMovement(std::chrono::duration<double> deltaTime);
	void InputToMenu();
	void InputToResults();
	void InputToGameMenu();

private:
	bool gpArr[16];
	//make these arrays at some point
	bool forward[17];
	bool backward[17];
	bool left[17];
	bool right[17];
	int shoot[17];
	bool reverse[17];
	int parry[17];
	int confirm[17];
	bool camLeft[17];
	bool camRight[17];
	int pause[17];
	const float sens = 0.5f; //controller joystick and trigger sensitivity
	const float gasPedal = 1;
	//to make sure menu left and menu right only press for one frame
	bool menuLeftPressed = false;
	bool menuRightPressed = false;
	bool menuUpPressed = false;
	bool menuDownPressed = false;

	bool backwards = false;

	float turnSpeed = 1.5;

	//mouse related things
	//bool initMouse = false;
	//double xpos, ypos, prevx, prevy, initx, inity;
	//int screenWidth;
	//bool mouseControl;
};