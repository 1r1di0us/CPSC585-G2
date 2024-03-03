#include "InputSystem.h"

InputSystem::InputSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;
	brakeTimer = 0.0;

	for (int i = 0; i < 16; i++) InputSystem::gpArr[i] = 0; //This is how you initialize an array. I can hardly believe it.
	for (int i = 0; i < 17; i++) {
		InputSystem::forward[i] = false;
		InputSystem::backward[i] = false;
		InputSystem::left[i] = false;
		InputSystem::right[i] = false;
		InputSystem::shoot[i] = 0;
	}
}

void InputSystem::getKeyboardInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward[0] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		backward[0] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		left[0] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right[0] = true;
	}

	//will shoot a projectile
	//FIXME: broken af rn. needs IO to be working to properly test
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (shoot[0] == 0) {
			shoot[0] = 1;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		if (shoot[0] == 2) {
			shoot[0] = 0;
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
					right[j+1] = true;
				}
				else if (x < -sens) {
					left[j+1] = true;
				}
				if (y > sens) {
					backward[j+1] = true;
				}
				else if (y < -sens) {
					forward[j+1] = true;
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
					if (shoot[j+1] == 0) {
						shoot[j+1] = 1;
					}
				}
				else if (x < -sens) {
					if (shoot[j+1] == 2) {
						shoot[j+1] = 0;
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

bool InputSystem::InputToMovement(std::chrono::duration<double> deltaTime) {

	//decrement timer
	if (brakeTimer < deltaTime.count()) {
		brakeTimer = 0.0;
	}
	else {
		brakeTimer -= deltaTime.count();
	}

	EngineDriveVehicle* playerCar = dataSys->GetVehicleFromRigidDynamic(dataSys->entityList[0].collisionBox);
	
	PxVec3 intentDir = { 0, 0, 0 };
	PxVec3 carDir = playerCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
	float carSpeed = playerCar->mPhysXState.physxActor.rigidBody->getLinearVelocity().magnitude();
	std::vector<int> checkvals = {0};
	for (int i = 0; i < 16; i++) {
		if (gpArr[i]) checkvals.push_back(i + 1);
	}
	bool f = false;
	bool b = false;
	bool l = false;
	bool r = false;
	for (int i : checkvals) if (forward[i]) {
		f = true;
		forward[i] = false;
	}
	for (int i : checkvals) if (backward[i]) {
		b = true;
		backward[i] = false;
	}
	for (int i : checkvals) if (left[i]) {
		l = true;
		left[i] = false;
	}
	for (int i : checkvals) if (right[i]) {
		r = true;
		right[i] = false;
	}
	if (f && !b) {
		
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(-1, 0, 0)).getNormalized();
	}
	else if (b && !f) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(1, 0, 0)).getNormalized();
	}

	if (l && !r) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(0, 0, 1)).getNormalized();
	}
	else if (r && !l) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + PxVec3(0, 0, -1)).getNormalized();
	}

	if (!r && !l && !f && !b) {
		playerCar->mCommandState.throttle = 0;
		playerCar->mCommandState.nbBrakes = 1;
		playerCar->mCommandState.brakes[0] = 1;
	}

	if (intentDir != PxVec3(0, 0, 0)) {
		float dot = carDir.dot(intentDir);
		float det = PxVec3(0, 1, 0).dot(carDir.cross(intentDir)); //triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
		float angle = atan2(dot, det);

		if (angle <= M_PI / 8 && angle >= -M_PI / 8) {
			playerCar->mCommandState.steer = -angle;
		}
		else if (angle < -M_PI/8) {
			playerCar->mCommandState.steer = 1;
			if (angle < -M_PI / 5 && brakeTimer == 0.0) {
				if (carSpeed > 20.0) {
					brakeTimer = 0.35;
				}
				else if (carSpeed > 18.0) {
					brakeTimer = 0.1;
				}
			}
		}
		else if (angle > M_PI/8) {
			playerCar->mCommandState.steer = -1;
			if (angle > M_PI / 5 && brakeTimer == 0.0) {
				if (carSpeed > 20.0) {
					brakeTimer = 0.35;
				}
				else if (carSpeed > 18.0) {
					brakeTimer = 0.1;
				}
			}
		}
		
		if (brakeTimer > 0.0) {
			if (carSpeed < 1.0) {
				brakeTimer = 0.0;
			}
			else {
				playerCar->mCommandState.throttle = 0;
				playerCar->mCommandState.nbBrakes = 1;
				playerCar->mCommandState.brakes[0] = 1;
			}
		}
	}

	int s = 0;
	for (int i : checkvals) if (shoot[i] == 1) {
		s = 1;
		shoot[i] = 2;
	}

	if (s == 1) {
		
		return true;
	}
	else {
		return false;
	}
}

	/*if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		; //gamepad buttons
	}*/