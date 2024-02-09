#include "InputSystem.h"

//this is a static function, as seen in InputSystem.h
void InputSystem::updateKeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE) {
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}
	// do not try storing what keys are pressed in any sort of variable unless you are very smart
	std::cout << "PRESSED: ";
	if (action == GLFW_PRESS) {
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
			std::cout << "right ";
		}
	}
	std::cout << "  RELEASED: ";
	if (action == GLFW_RELEASE) {
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
			std::cout << "right ";
		}
	}
};

void InputSystem::checkIfGamepadsPresent() {
	for (int j = 0; j < 16; j++) {
		if (glfwJoystickIsGamepad(j)) {
			gpArr[j] = true;
		}
		else {
			gpArr[j] = false;
		}
	}
}

void InputSystem::getGamePadInput() {
	GLFWgamepadstate state;
	for (int j = 0; j < 16; j++) {
		if (gpArr[j]) {
			if (glfwGetGamepadState(j, &state)) {
				std::cout << "  GAMEPAD " << j + 1 << ": ";
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
					std::cout << "a ";
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
					std::cout << "b ";
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) {
					std::cout << "x ";
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS) {
					std::cout << "y ";
				}
				std::cout << "  JOYSTICK L: ";
				float x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				float y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				if (x > 0.9f) {
					std::cout << "right ";
				}
				else if (x < -sens) {
					std::cout << "left ";
				}
				else {
					std::cout << "mid ";
				}
				if (y > sens) {
					std::cout << "down ";
				}
				else if (y < -sens) {
					std::cout << "up ";
				}
				else {
					std::cout << "mid ";
				}
				std::cout << "  JOYSTICK R: ";
				x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
				y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
				if (x > sens) {
					std::cout << "right ";
				}
				else if (x < -sens) {
					std::cout << "left ";
				}
				else {
					std::cout << "mid ";
				}
				if (y > sens) {
					std::cout << "down ";
				}
				else if (y < -sens) {
					std::cout << "up ";
				}
				else {
					std::cout << "mid ";
				}

			}
		}
	}
	
}

	/*if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		; //gamepad buttons
	}*/