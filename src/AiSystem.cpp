#include "AiSystem.h"

//DELETE THIS
enum State { SIT, SPIN, MOVETO };  //no idea how to do what I want

AiSystem::AiSystem(SharedDataSystem* dataSys) {
	this->dataSys = dataSys;
	timer = 0.0;
	state = MOVETO;
	navMesh = new NavMesh();
	pathFinder = new PathFinder(navMesh);
	moveLocation = PxVec3(0, 0, 0);
}

bool AiSystem::update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime) {
	int ammo = 0;
	bool fire = false;
	if (ammo > 1) fire = hunting_behaviour(aiCar, fire);
	else fire = hiding_behaviour(aiCar, fire);
	if (moveLocation != PxVec3(0, 0, 0)) astar_path_finding(aiCar);
}

void astar_path_finding(EngineDriveVehicle* aiCar) {
	//i don't know how to do it.
}


bool AiSystem::update_old(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime, PxVec3 movLoc) {
	moveLocation = movLoc;
	bool fire = false;
	//update timers
	if (timer < deltaTime.count()) {
		timer = 0.0;
	} else {
		timer -= deltaTime.count();
	}
	if (brakeTimer < deltaTime.count()) {
		brakeTimer = 0.0;
	}
	else {
		brakeTimer -= deltaTime.count();
	}

	if (aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p.magnitude() > 50.f) { //leashes the ai to the origin point
		state = MOVETO;
	}

	if (state == SIT) {
		fire = sit_behaviour(aiCar, fire);
	}
	else if (state == SPIN) {
		fire = spin_behaviour(aiCar, fire);
	}
	else if (state == MOVETO) {
		fire = moveto_behaviour(aiCar, fire);
	}
	return fire;
}

bool AiSystem::sit_behaviour(EngineDriveVehicle* aiCar, bool fire) {
	aiCar->mCommandState.steer = 0.f;
	aiCar->mCommandState.throttle = 0.f;
	aiCar->mCommandState.brakes[0] = 1.f;
	aiCar->mCommandState.nbBrakes = 1.f;
	if (timer <= 0.0) {
		fire = true;
		state = SPIN;
		distribution = std::normal_distribution<double>(3.0, 1.5);
		timer = distribution(rand); //generate a random number with normal distribution with mean of 5 and standard deviation of 2
	}
	return fire;
}

bool AiSystem::spin_behaviour(EngineDriveVehicle* aiCar, bool fire) {
	aiCar->mCommandState.steer = 1;
	aiCar->mCommandState.throttle = 0.8; //speeeeeeeeeeeeeeeeeen
	aiCar->mCommandState.brakes[0] = 0.f;
	aiCar->mCommandState.nbBrakes = 0.f;
	if (timer <= 0.0) {
		state = SIT;
		timer = 0.6;
	}
	return fire;
}

bool AiSystem::moveto_behaviour(EngineDriveVehicle* aiCar, bool fire) {

	PxVec3 carPos = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	float carSpeed = aiCar->mPhysXState.physxActor.rigidBody->getLinearVelocity().magnitude();
	float dist = (moveLocation - carPos).magnitude();
	PxVec3 intentDir = (moveLocation - carPos).getNormalized();
	float x = intentDir.x;
	intentDir.x = intentDir.z;
	intentDir.z = -x; //rotate it 90 degrees
	// I don't know how to explain why but it has to do with the car being able to turn more than 180 degrees
	PxVec3 carDir = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();

	float dot = carDir.dot(intentDir);
	float det = PxVec3(0, 1, 0).dot(carDir.cross(intentDir));
	//triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
	float angle = atan2(dot, det);

	if (dist < 2) {
		state = SIT;
		timer = 0.2;
	}
	else {
		if (angle <= M_PI / 8 && angle >= -M_PI / 8) {
			aiCar->mCommandState.steer = -angle;
		}
		else if (angle < -M_PI / 8) {
			aiCar->mCommandState.steer = 1;
			if (angle < -M_PI / 4 && brakeTimer == 0.0) {
				if (carSpeed > 19.0) {
					brakeTimer = 0.35;
				}
			}
		}
		else if (angle > M_PI / 8) {
			aiCar->mCommandState.steer = -1;
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
				aiCar->mCommandState.throttle = 0;
				aiCar->mCommandState.nbBrakes = 1;
				aiCar->mCommandState.brakes[0] = 1;
			}
		}
		else {
			aiCar->mCommandState.nbBrakes = 0.0f;
			aiCar->mCommandState.brakes[0] = 0.0f;
			aiCar->mCommandState.throttle = 1;
		}
	}
	return fire;
}