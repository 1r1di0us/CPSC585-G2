#include "AiSystem.h"

enum State { SIT, SPIN, MOVETO };  //no idea how to do what I want

AiSystem::AiSystem(SharedDataSystem* dataSys) {
	this->dataSys = dataSys;
	state = MOVETO;
}

//TODO: add randomness and timing.
//TODO: add shooting
//TODO: give it an ai car and test it
//TODO: ask matt why anyone would want to use quaternions to find the direction the guy is facing.
//TODO: actually think of an ai

void AiSystem::update(EngineDriveVehicle* aiCar) {
	if (state == SIT) {
		sit_behaviour(aiCar);
	}
	else if (state == SPIN) {
		spin_behaviour(aiCar);
	}
	else if (state == MOVETO) {
		moveto_behaviour(aiCar, PxVec3(0, 0, 0));
	}
}

void AiSystem::sit_behaviour(EngineDriveVehicle* aiCar) {
	aiCar->mCommandState.steer = 0.f;
	aiCar->mCommandState.throttle = 0.f;
	aiCar->mCommandState.brakes[0] = 1.f;
	aiCar->mCommandState.nbBrakes = 1.f;
	//would love to make it a turret that shoots periodically but i can't turn the turret
}

void AiSystem::spin_behaviour(EngineDriveVehicle* aiCar) {
	aiCar->mCommandState.steer = 2.5;
	aiCar->mCommandState.throttle = 1; //speeeeeeeeeeeeeeeeeen
	aiCar->mCommandState.brakes[0] = 0.f;
	aiCar->mCommandState.nbBrakes = 0.f;
}

void AiSystem::moveto_behaviour(EngineDriveVehicle* aiCar, PxVec3 goal) {

	PxVec3 carPos = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	float dist = (goal - carPos).magnitude();
	PxVec3 intentDir = (goal - carPos).getNormalized();
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
		state = SPIN;
		return;
	}
	else if (dist < 3) {
		aiCar->mCommandState.nbBrakes = 1.0f;
		aiCar->mCommandState.brakes[0] = 1.0f;
		aiCar->mCommandState.throttle = 0.f;
	}
	else {
		if (angle <= M_PI / 8 && angle >= -M_PI / 8) {
			aiCar->mCommandState.steer = -4 * angle;
		}
		else if (angle > -M_PI / 8) {
			aiCar->mCommandState.steer = -2.5;
		}
		else if (angle < M_PI / 8) {
			aiCar->mCommandState.steer = 2.5;
		}
		aiCar->mCommandState.nbBrakes = 0.0f;
		aiCar->mCommandState.brakes[0] = 0.0f;
		aiCar->mCommandState.throttle = 1;
	}
}