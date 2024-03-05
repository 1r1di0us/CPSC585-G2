#include "InputSystem.h"

InputSystem::InputSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;
	brakeTimer = 0.0;

	for (int i = 0; i < 16; i++) InputSystem::gpArr[i] = 0; //This is how you initialize an array. I can hardly believe it.
	for (int i = 0; i < 17; i++) {
		forward[i] = false;
		backward[i] = false;
		left[i] = false;
		right[i] = false;
		confirm[i] = 0;
		shoot[i] = 0;
		reverse[i] = false;
		camLeft[i] = false;
		camRight[i] = false;
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

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		reverse[0] = true;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camLeft[0] = true;
		mouseControl = false; //if you press keys and not move mouse you get pan control not mouse control
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camRight[0] = true;
		mouseControl = false;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (dataSys->inMenu || dataSys->inResults) {
			if (confirm[0] == 0) confirm[0] = 1;
			shoot[0] = 3; //won't shoot if its 3
		}
		else {
			if (shoot[0] == 0) shoot[0] = 1;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		if (dataSys->inMenu || dataSys->inResults) {
			if (confirm[0] >= 2) {
				confirm[0] = 0;
			}
		}
		else {
			if (shoot[0] >= 2) shoot[0] = 0;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) { //toggle birds eye view
		if (dataSys->useBirdsEyeView == 0) {
			dataSys->useBirdsEyeView = 1;
		}
		else if (dataSys->useBirdsEyeView == 2) {
			dataSys->useBirdsEyeView = 3;
		}
	} 
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		if (dataSys->useBirdsEyeView == 1) {
			dataSys->useBirdsEyeView = 2;
		}
		else if (dataSys->useBirdsEyeView == 3) {
			dataSys->useBirdsEyeView = 0;
		}
	}

	prevx = xpos;
	prevy = ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (initMouse) {
		initMouse = false;
		initx = xpos;
		inity = ypos;
	}
	if (xpos != prevx || ypos != prevy) { //if you move mouse you get mouse control, overrides pan control
		mouseControl = true;
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

// Need to add confirm button on controller
void InputSystem::getGamePadInput() {
	GLFWgamepadstate state;
	for (int j = 0; j < 16; j++) {
		if (gpArr[j]) {
			if (glfwGetGamepadState(j, &state)) {
				//movement, left joystick
				float x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				float y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				if (x < -sens) {
					left[j+1] = true;
				}
				else if (x > sens) {
					right[j+1] = true;
				}
				if (y < -sens) {
					forward[j+1] = true;
				}
				else if (y > sens) {
					backward[j+1] = true;
				}
				//camera
				x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
				//y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
				if (x < -sens) {
					camLeft[j + 1] = true;
					mouseControl = false;
				}
				else if (x > sens) {
					camRight[j + 1] = true;
					mouseControl = false;
				}
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
					if (dataSys->inMenu || dataSys->inResults) {
						if (confirm[j + 1] == 0) confirm[j + 1] = 1;
						shoot[0] = 3;
					}
					else {
						if (shoot[j + 1] == 0) shoot[j + 1] = 1;
					}
				}
				else if (x < -sens) {
					if (dataSys->inMenu || dataSys->inResults) {
						if (confirm[j + 1] >= 2) confirm[j + 1] = 0;
					}
					else {
						if (shoot[j + 1] >= 2) shoot[j + 1] = 0;
					}
				}
				if (y >= sens) { //right trigger
					reverse[j + 1] = true;
				}
				else if (y < -sens) {
					//???
				}
			}
		}
	}
	
}

bool InputSystem::InputToMovement(std::chrono::duration<double> deltaTime) {
	//update timer
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
	bool rev = false;
	bool cl = false;
	bool cr = false;
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
	for (int i : checkvals) if (reverse[i]) {
		rev = true;
		reverse[i] = false;
	}
	for (int i : checkvals) if (camLeft[i]) {
		cl = true;
		camLeft[i] = false;
	}
	for (int i : checkvals) if (camRight[i]) {
		cr = true;
		camRight[i] = false;
	}

	if (f && !b) {
		
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + dataSys->getCamRotMatPx(M_PI - dataSys->cameraAngle) * PxVec3(-1, 0, 0)).getNormalized();
	}
	else if (b && !f) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + dataSys->getCamRotMatPx(M_PI - dataSys->cameraAngle) * PxVec3(1, 0, 0)).getNormalized();
	}

	if (l && !r) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + dataSys->getCamRotMatPx(dataSys->cameraAngle) * PxVec3(0, 0, 1)).getNormalized();
	}
	else if (r && !l) {
		playerCar->mCommandState.throttle = gasPedal;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
		intentDir = (intentDir + dataSys->getCamRotMatPx(dataSys->cameraAngle) * PxVec3(0, 0, -1)).getNormalized();
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
			if (angle < -M_PI / 4 && brakeTimer == 0.0) {
				if (carSpeed > 19.0) {
					brakeTimer = 0.35;
				}
			}
		}
		else if (angle > M_PI/8) {
			playerCar->mCommandState.steer = -1;
			if (angle > M_PI / 4 && brakeTimer == 0.0) {
				if (carSpeed > 19.0) {
					brakeTimer = 0.35;
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

	//reverse overrides all
	if (rev) {
		playerCar->mTransmissionCommandState.targetGear = 0;
		playerCar->mCommandState.steer = 0;
		playerCar->mCommandState.throttle = 1;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
	}
	else {
		playerCar->mTransmissionCommandState.targetGear = 2;
	}

	//camera shenanigans	
	if (cl && !cr) {
		dataSys->cameraAngle += 1.5 * deltaTime.count();
	}
	else if (cr && !cl) {
		dataSys->cameraAngle -= 1.5 * deltaTime.count();
	}


	if (mouseControl) {
		dataSys->cameraAngle = fmod(M_PI * ((initx-xpos) / 1600), 2 * M_PI);
	}
	else {
		dataSys->cameraAngle = fmod(dataSys->cameraAngle, 2 * M_PI);
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

void InputSystem::InputToMenu() {
	std::vector<int> checkvals = { 0 };
	for (int i = 0; i < 16; i++) {
		if (gpArr[i]) checkvals.push_back(i + 1);
	}

	bool l = false;
	bool r = false;
	bool conf = false;

	for (int i : checkvals) if (left[i]) {
		l = true;
		left[i] = false;
	}
	for (int i : checkvals) if (right[i]) {
		r = true;
		right[i] = false;
	}
	for (int i : checkvals) if (confirm[i] == 1) {
		conf = true;
		confirm[i] = 2;
	}

	// Check if left key is pressed and was not pressed before
	if (l && !menuLeftPressed) {
		dataSys->menuOptionIndex = (dataSys->menuOptionIndex - 1) % dataSys->nbMenuOptions;
		menuLeftPressed = true;
	}
	else if (!l) {
		menuLeftPressed = false;
	}

	// Check if right key is pressed and was not pressed before
	if (r && !menuRightPressed) {
		dataSys->menuOptionIndex = (dataSys->menuOptionIndex + 1) % dataSys->nbMenuOptions;
		menuRightPressed = true;
	}
	else if (!r) {
		menuRightPressed = false;
	}

	if (conf && dataSys->menuOptionIndex == 0) {
		initMouse = true;
		dataSys->inMenu = false;
	}
	else if (conf && dataSys->menuOptionIndex == 1) {
		dataSys->quit = true;
	}

	if (dataSys->menuOptionIndex < 0) {
		dataSys->menuOptionIndex = dataSys->nbMenuOptions - 1;
	}
}

void InputSystem::InputToResults() {
	std::vector<int> checkvals = { 0 };
	for (int i = 0; i < 16; i++) {
		if (gpArr[i]) checkvals.push_back(i + 1);
	}

	bool conf = false;

	for (int i : checkvals) if (confirm[i] == 1) {
		conf = true;
		confirm[i] = 2;
	}

	if (conf) {
		dataSys->inMenu = true;
		dataSys->inResults = false;

		dataSys->winningPlayer = 0;
		dataSys->tieGame = false;
		dataSys->carsInitialized = false;
		dataSys->resetSharedDataSystem();
	}
}

	/*if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		; //gamepad buttons
	}*/