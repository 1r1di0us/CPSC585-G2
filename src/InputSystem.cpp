#include "InputSystem.h"

InputSystem::InputSystem(Entity* pcar) {
	playerCar = pcar;
	for (int i = 0; i < 16; i++) InputSystem::gpArr[i] = 0; //This is how you initialize an array. I can hardly believe it.
}

void InputSystem::getKeyboardInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
		forward = false;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		backward = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		backward = false;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		left = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
		left = false;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		right = false;
	}

	if (forward && !backward) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
	}
	else if (backward && !forward) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mTransmissionCommandState.targetGear = PxVehicleGearsData::eREVERSE;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
	}
	else {
		playerCar->car->gVehicle.mCommandState.throttle = 0;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 1;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 1;
	}

	if (left && !right) {
		playerCar->car->gVehicle.mCommandState.steer = 1;
	}
	else if (right && !left) {
		playerCar->car->gVehicle.mCommandState.steer = -1;
	}
	else {
		playerCar->car->gVehicle.mCommandState.steer = 0;
	}

	//will shoot a projectile
	//FIXME: broken af rn. needs IO to be working to properly test
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//shoot(&playerCar);
	}
}

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