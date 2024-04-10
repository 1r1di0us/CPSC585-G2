#include "InputSystem.h"

InputSystem::InputSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;

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
		pause[i] = 0;
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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (dataSys->inMenu || dataSys->inResults || dataSys->inGameMenu) {
			if (confirm[0] == 0) confirm[0] = 1;
			shoot[0] = 3; //won't shoot if its 3
		}
		else {
			if (shoot[0] == 0) shoot[0] = 1;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		if (dataSys->inMenu || dataSys->inResults || dataSys->inGameMenu) {
			if (confirm[0] >= 2) {
				confirm[0] = 0;
			}
		}
		else {
			if (shoot[0] >= 2) shoot[0] = 0;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (pause[0] == 0) pause[0] = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		if (pause[0] >= 2) {
			pause[0] = 0;
		}
	}

	// Disable unused buttons in pause menu
	if (!dataSys->inGameMenu) {
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			reverse[0] = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			camLeft[0] = true;
			//mouseControl = false; //if you press keys and not move mouse you get pan control not mouse control
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			camRight[0] = true;
			//mouseControl = false;
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			if (parry[0] == 0) parry[0] = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
			if (parry[0] >= 2) parry[0] = 0;
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

		//prevx = xpos;
		//prevy = ypos;
		//glfwGetCursorPos(window, &xpos, &ypos);
		//if (initMouse) {
		//	initMouse = false;
		//	initx = xpos;
		//	inity = ypos;
		//}
		//if (xpos != prevx || ypos != prevy) { //if you move mouse you get mouse control, overrides pan control
		//	mouseControl = true;
		//}
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
				
				if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
					if (confirm[j + 1] == 0) confirm[j + 1] = 1;
				}
				else if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE) {
					if (confirm[j + 1] >= 2) confirm[j + 1] = 0;
				}
				if (!dataSys->inGameMenu) {
					//camera
					x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
					//y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
					if (x < -sens) {
						camLeft[j + 1] = true;
						//mouseControl = false;
					}
					else if (x > sens) {
						camRight[j + 1] = true;
						//mouseControl = false;
					}

					x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]; // too lazy to make new variables
					y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
					if (x >= sens) { //left trigger
						if (!dataSys->inMenu || !dataSys->inResults) {
							if (shoot[j + 1] == 0) shoot[j + 1] = 1;
						}
					}
					else if (x < -sens) {
						if (!dataSys->inMenu || !dataSys->inResults) {
							if (shoot[j + 1] >= 2) shoot[j + 1] = 0;
						}
					}
					if (y >= sens) { //right trigger
						reverse[j + 1] = true;
					}
					else if (y < -sens) {
						//???
					}

					if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
						if (parry[j + 1] == 0) parry[j + 1] = 1;
					}
					else if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_RELEASE) {
						if (parry[j + 1] >= 2) parry[j + 1] = 0;
					}
				}
				if (state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
					if (pause[j + 1] == 0) pause[j + 1] = 1;
				}
				else if (state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_RELEASE) {
					if (pause[j + 1] >= 2) pause[j + 1] = 0;
				}
			}
		}
	}
	
}

int InputSystem::InputToMovement(std::chrono::duration<double> deltaTime) {

	EngineDriveVehicle* playerCar = dataSys->GetVehicleFromRigidDynamic(dataSys->carInfoList[0].entity->collisionBox);
	
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
	int s = 0;
	int p = 0;
	bool cl = false;
	bool cr = false;
	bool pauseGame = false;
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
	for (int i : checkvals) if (shoot[i] == 1) {
		s = 1;
		shoot[i] = 2;
	}
	for (int i : checkvals) if (parry[i] == 1) {
		p = 1;
		parry[i] = 2;
	}
	for (int i : checkvals) if (camLeft[i]) {
		cl = true;
		camLeft[i] = false;
	}
	for (int i : checkvals) if (camRight[i]) {
		cr = true;
		camRight[i] = false;
	}
	for (int i : checkvals) if (pause[i] == 1) {
		pauseGame = true;
		pause[i] = 2;
	}

	if (f && !b) {
		intentDir = (intentDir + dataSys->getRotMatPx(M_PI - dataSys->cameraAngle) * PxVec3(-1, 0, 0)).getNormalized();
	}
	else if (b && !f) {
		intentDir = (intentDir + dataSys->getRotMatPx(M_PI - dataSys->cameraAngle) * PxVec3(1, 0, 0)).getNormalized();
	}

	if (l && !r) {
		intentDir = (intentDir + dataSys->getRotMatPx(dataSys->cameraAngle) * PxVec3(0, 0, 1)).getNormalized();
	}
	else if (r && !l) {
		intentDir = (intentDir + dataSys->getRotMatPx(dataSys->cameraAngle) * PxVec3(0, 0, -1)).getNormalized();
	}

	if (!r && !l && !f && !b) { //when no move == stop
		playerCar->mCommandState.throttle = 0;
		playerCar->mCommandState.nbBrakes = 1;
		playerCar->mCommandState.brakes[0] = 1;
	}

	if (intentDir != PxVec3(0, 0, 0)) {
		//get the angle between the intentDir and the carDir
		float dot = carDir.dot(intentDir);
		float det = PxVec3(0, 1, 0).dot(carDir.cross(intentDir));
		//triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
		float angle = atan2(dot, det);

		//if (playerCar->mTransmissionCommandState.targetGear == 0) playerCar->mTransmissionCommandState.targetGear = 2;

		if (backwards == true) {
			if (angle > -1 * M_PI / 4 && angle < 1 * M_PI / 4) {
				backwards = false;
			}
			else if (angle <= -M_PI + 1) {
				playerCar->mCommandState.steer = angle + M_PI;
				playerCar->mCommandState.throttle = 1;
				playerCar->mCommandState.nbBrakes = 0;
				playerCar->mCommandState.brakes[0] = 0;
			}
			else if (angle >= M_PI - 1) {
				playerCar->mCommandState.steer = angle - M_PI;
				playerCar->mCommandState.throttle = 1;
				playerCar->mCommandState.nbBrakes = 0;
				playerCar->mCommandState.brakes[0] = 0;
			}
			else if (angle < M_PI - 1 && angle > 0) {
				playerCar->mCommandState.steer = -1;
				if (carSpeed > 15.0) {
					playerCar->mCommandState.throttle = 0;
					playerCar->mCommandState.nbBrakes = 1;
					playerCar->mCommandState.brakes[0] = 1;
				}
				else {
					playerCar->mCommandState.throttle = 1;
					playerCar->mCommandState.nbBrakes = 0;
					playerCar->mCommandState.brakes[0] = 0;
				}
			}
			else if (angle > -M_PI + 1 && angle < 0) {
				playerCar->mCommandState.steer = 1;
				if (carSpeed > 15.0) {
					playerCar->mCommandState.throttle = 0;
					playerCar->mCommandState.nbBrakes = 1;
					playerCar->mCommandState.brakes[0] = 1;
				}
				else {
					playerCar->mCommandState.throttle = 1;
					playerCar->mCommandState.nbBrakes = 0;
					playerCar->mCommandState.brakes[0] = 0;
				}
			}
		}
		else {
			if (angle > 3 * M_PI / 4 || angle < -3 * M_PI / 4) {
				backwards = true;
			}
			else if (angle <= 1 && angle >= -1) {
				playerCar->mCommandState.steer = -angle;
				playerCar->mCommandState.throttle = 1;
				playerCar->mCommandState.nbBrakes = 0;
				playerCar->mCommandState.brakes[0] = 0;
			}
			else if (angle < -1) {
				playerCar->mCommandState.steer = 1;
				if (carSpeed > 15.0) {
					playerCar->mCommandState.throttle = 0;
					playerCar->mCommandState.nbBrakes = 1;
					playerCar->mCommandState.brakes[0] = 1;
				}
				else {
					playerCar->mCommandState.throttle = 1;
					playerCar->mCommandState.nbBrakes = 0;
					playerCar->mCommandState.brakes[0] = 0;
				}
			}
			else if (angle > 1) {
				playerCar->mCommandState.steer = -1;
				if (carSpeed > 15.0) {
					playerCar->mCommandState.throttle = 0;
					playerCar->mCommandState.nbBrakes = 1;
					playerCar->mCommandState.brakes[0] = 1;
				}
				else {
					playerCar->mCommandState.throttle = 1;
					playerCar->mCommandState.nbBrakes = 0;
					playerCar->mCommandState.brakes[0] = 0;
				}
			}
		}
		/*if (angle > 3 * M_PI / 4 || angle < -3 * M_PI / 4) {
				PxVec3 myDir = -carDir * 10;
				myDir.y = 0;
				dataSys->GetRigidDynamicFromVehicle(playerCar)->setLinearVelocity(myDir);
				playerCar->mTransmissionCommandState.targetGear = 0;
				playerCar->mCommandState.steer = -angle;
				backwards = true;
		}
		else if (angle <= 1 && angle >= -1) {
			playerCar->mCommandState.steer = -angle;
			playerCar->mCommandState.throttle = 1;
			playerCar->mCommandState.nbBrakes = 0;
			playerCar->mCommandState.brakes[0] = 0;
		}
		else if (angle < -1) {
			playerCar->mCommandState.steer = 1;
			if (carSpeed > 15.0) {
				playerCar->mCommandState.throttle = 0;
				playerCar->mCommandState.nbBrakes = 1;
				playerCar->mCommandState.brakes[0] = 1;
			}
			else {
				playerCar->mCommandState.throttle = 1;
				playerCar->mCommandState.nbBrakes = 0;
				playerCar->mCommandState.brakes[0] = 0;
			}
		}
		else if (angle > 1) {
			playerCar->mCommandState.steer = -1;
			if (carSpeed > 15.0) {
				playerCar->mCommandState.throttle = 0;
				playerCar->mCommandState.nbBrakes = 1;
				playerCar->mCommandState.brakes[0] = 1;
			}
			else {
				playerCar->mCommandState.throttle = 1;
				playerCar->mCommandState.nbBrakes = 0;
				playerCar->mCommandState.brakes[0] = 0;
			}
		}
		if (angle > -1 * M_PI / 4 && angle < 1 * M_PI / 4) {
			backwards = false;
		}*/
	}

	//reverse overrides all
	/*if (rev) {
		if (backwards == false && playerCar->mTransmissionCommandState.targetGear == 2) {
			PxVec3 myDir = -carDir * 10;
			myDir.y = 0;
			dataSys->GetRigidDynamicFromVehicle(playerCar)->setLinearVelocity(myDir);
			playerCar->mTransmissionCommandState.targetGear = 0;
		}
		if (backwards == true && playerCar->mTransmissionCommandState.targetGear == 0) {
			PxVec3 myDir = carDir * 10;
			myDir.y = 0;
			dataSys->GetRigidDynamicFromVehicle(playerCar)->setLinearVelocity(myDir);
			playerCar->mTransmissionCommandState.targetGear = 2;
		}
		playerCar->mCommandState.steer = 0;
		playerCar->mCommandState.throttle = 1;
		playerCar->mCommandState.nbBrakes = 0;
		playerCar->mCommandState.brakes[0] = 0;
	}
	else {*/
	if (playerCar->mTransmissionCommandState.targetGear == 0 && backwards == false) {
		PxVec3 myDir = carDir * 10;
		myDir.y = 0;
		dataSys->GetRigidDynamicFromVehicle(playerCar)->setLinearVelocity(myDir);
		playerCar->mTransmissionCommandState.targetGear = 2;
	}
	if (playerCar->mTransmissionCommandState.targetGear == 2 && backwards == true) {
		PxVec3 myDir = -carDir * 10;
		myDir.y = 0;
		dataSys->GetRigidDynamicFromVehicle(playerCar)->setLinearVelocity(myDir);
		playerCar->mTransmissionCommandState.targetGear = 0;
	}
	printf("speed: %f\n", playerCar->mPhysXState.physxActor.rigidBody->getLinearVelocity().normalize());
	//}

	//camera shenanigans	
	if (cl && !cr) {
		dataSys->cameraAngle += 1.5 * deltaTime.count();
	}
	else if (cr && !cl) {
		dataSys->cameraAngle -= 1.5 * deltaTime.count();
	}


	//if (mouseControl) {
	//	dataSys->cameraAngle = fmod(M_PI * ((initx-xpos) / 1600), 2 * M_PI);
	//}
	//else {
	//	dataSys->cameraAngle = fmod(dataSys->cameraAngle, 2 * M_PI);
	//}

	//shoot
	if (s == 1) {
		return 1;
	}

	//parry
	if (p == 1) {
		return 2;
	}

	if (pauseGame) {
		dataSys->inGameMenu = true;
	}

	//if no action is required
	return 0;
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

	if (conf && dataSys->inControlsMenu) {
		dataSys->menuOptionIndex = 0;
		dataSys->inControlsMenu = false;
	}

	else if (conf && dataSys->menuOptionIndex == 0 && !dataSys->inControlsMenu){
		//initMouse = true;
		dataSys->inMenu = false;
	}
	else if (conf && dataSys->menuOptionIndex == 1 && !dataSys->inControlsMenu) {
		dataSys->inControlsMenu = true;
	}
	else if (conf && dataSys->menuOptionIndex == 2 && !dataSys->inControlsMenu) {
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

void InputSystem::InputToGameMenu() {
	std::vector<int> checkvals = { 0 };
	for (int i = 0; i < 16; i++) {
		if (gpArr[i]) checkvals.push_back(i + 1);
	}

	bool u = false;
	bool d = false;
	bool conf = false;
	bool unpause = false;

	for (int i : checkvals) if (forward[i]) {
		u = true;
		forward[i] = false;
	}
	for (int i : checkvals) if (backward[i]) {
		d = true;
		backward[i] = false;
	}
	for (int i : checkvals) if (confirm[i] == 1) {
		conf = true;
		confirm[i] = 2;
	}
	for (int i : checkvals) if (pause[i] == 1) {
		unpause = true;
		pause[i] = 2;
	}

	// Check if left key is pressed and was not pressed before
	if (u && !menuUpPressed) {
		dataSys->ingameOptionIndex = (dataSys->ingameOptionIndex - 1) % dataSys->nbIngameOptions;
		menuUpPressed = true;
	}
	else if (!u) {
		menuUpPressed = false;
	}

	// Check if right key is pressed and was not pressed before
	if (d && !menuDownPressed) {
		dataSys->ingameOptionIndex = (dataSys->ingameOptionIndex + 1) % dataSys->nbIngameOptions;
		menuDownPressed = true;
	}
	else if (!d) {
		menuDownPressed = false;
	}

	if ((conf && dataSys->ingameOptionIndex == 0) || (unpause)) {
		dataSys->inGameMenu = false;
	}
	else if (conf && dataSys->ingameOptionIndex == 1) {
		dataSys->ingameOptionIndex = 0;
		dataSys->inGameMenu = false;
		dataSys->inMenu = true;

		dataSys->winningPlayer = 0;
		dataSys->tieGame = false;
		dataSys->carsInitialized = false;
		dataSys->resetSharedDataSystem();
	}

	if (dataSys->ingameOptionIndex < 0) {
		dataSys->ingameOptionIndex = dataSys->nbIngameOptions - 1;
	}
}
	/*if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		; //gamepad buttons
	}*/