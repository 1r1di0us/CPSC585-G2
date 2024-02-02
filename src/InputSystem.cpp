#include "InputSystem.h"

//this is a static function, as seen in InputSystem.h
void InputSystem::updateKeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}
	// do not try storing what keys are pressed in any sort of variable unless you are very smart,
	if (action == GLFW_PRESS) {
		std::cout << "PRESSED: ";
		if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
			std::cout << "up ";
		}
		if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
			std::cout << "down ";
		}
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
			std::cout << "left ";
		}
		if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
			std::cout << "right";
		}
		std::cout << std::endl;
	}
	if (action == GLFW_RELEASE) {
		std::cout << "RELEASED: ";
		if (key == GLFW_KEY_UP || key == GLFW_KEY_W) {
			std::cout << "up ";
		}
		if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S) {
			std::cout << "down ";
		}
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
			std::cout << "left ";
		}
		if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
			std::cout << "right";
		}
		std::cout << std::endl;
	}
}