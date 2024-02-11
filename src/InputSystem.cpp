#include "InputSystem.h"

InputSystem::InputSystem(Entity* pcar) {
	playerCar = pcar;
	for (int i = 0; i < 16; i++) InputSystem::gpArr[i] = 0; //This is how you initialize an array. I can hardly believe it.
}

void InputSystem::getKeyboardInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		backward = true;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		left = true;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right = true;
	}

	//will shoot a projectile
	//FIXME: broken af rn. needs IO to be working to properly test
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (shoot == 0) {
			shoot = 1;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		if (shoot == 2) {
			shoot = 0;
		}
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
				//movement, left joystick
				float x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				float y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				if (x > 0.9f) {
					right = true;
				}
				else if (x < -sens) {
					left = true;
				}
				if (y > sens) {
					backward = true;
				}
				else if (y < -sens) {
					forward = true;
				}
				//camera
				//x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
				//y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
				//if (x > sens) {
				//	std::cout << "right ";
				//}
				//else if (x < -sens) {
				//	std::cout << "left ";
				//}
				//else {
				//	std::cout << "mid ";
				//}
				//if (y > sens) {
				//	std::cout << "down ";
				//}
				//else if (y < -sens) {
				//	std::cout << "up ";
				//}
				//else {
				//	std::cout << "mid ";
				//}
				x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]; // too lazy to make new variables
				y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
				if (x >= sens) { //left trigger
					if (gpshoot == 0) {
						gpshoot = 1;
					}
				}
				else if (x < -sens) {
					if (gpshoot == 2) {
						gpshoot = 0;
					}
				}
				if (y >= sens) { //right trigger
					//?
				}
				else if (y < -sens) {
					//idk
				}
			}
		}
	}
	
}

void InputSystem::InputToMovement() {
	PxVec3 intentDir = { 0, 0, 0 };
	PxVec3 carDir = playerCar->car->gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
	if (forward && !backward) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(-1, 0, 0)).getNormalized();
	}
	else if (backward && !forward) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(1, 0, 0)).getNormalized();
	}

	if (left && !right) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(0, 0, 1)).getNormalized();
	}
	else if (right && !left) {
		playerCar->car->gVehicle.mCommandState.throttle = 1;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 0;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(0, 0, -1)).getNormalized();
	}

	if (!right && !left && !forward && !backward) {
		playerCar->car->gVehicle.mCommandState.throttle = 0;
		playerCar->car->gVehicle.mCommandState.nbBrakes = 1;
		playerCar->car->gVehicle.mCommandState.brakes[0] = 1;
	}
	forward = backward = left = right = false;
	if (intentDir != PxVec3(0, 0, 0)) {
		float dot = carDir.dot(intentDir);
		float det = PxVec3(0, 1, 0).dot(carDir.cross(intentDir)); //triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
		float angle = atan2(dot, det);

		if (angle <= M_PI / 8 && angle >= -M_PI / 8) {
			playerCar->car->gVehicle.mCommandState.steer = -4*angle;
		}
		else if (angle > -M_PI/8) {
			playerCar->car->gVehicle.mCommandState.steer = -2.5;
		}
		else if (angle < M_PI/8) {
			playerCar->car->gVehicle.mCommandState.steer = 2.5;
		}
	}

	if (shoot == 1) {
		playerCar->car->shootProjectile();

		//making a new transform every time projectile is shot
		playerCar->projectileTransformList.emplace_back(new Transform());
		shoot = 2;
	}
	if (gpshoot == 1) {
		playerCar->car->shootProjectile();

		//making a new transform every time projectile is shot
		playerCar->projectileTransformList.emplace_back(new Transform());
		gpshoot = 2;
	}
}

	/*if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		; //gamepad buttons
	}*/