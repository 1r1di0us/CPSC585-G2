#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <iostream> //for now

class InputSystem {
public:
	static void updateKeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods);
};