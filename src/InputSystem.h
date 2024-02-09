#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now
#include <math.h>

class InputSystem {
public:
	static void updateKeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods);
	void checkIfGamepadsPresent();
	void getGamePadInput();
private:
	bool gpArr[16];
	const float sens = 0.5f; //controller joystick sensitivity
};