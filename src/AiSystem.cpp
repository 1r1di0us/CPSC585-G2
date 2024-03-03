#include "AiSystem.h"

enum State { SIT, SPIN, MOVETO };  //no idea how to do what I want

AiSystem::AiSystem(SharedDataSystem* dataSys) {
	this->dataSys = dataSys;
	timer = 0.0;
	state = MOVETO;

}

//TODO: add randomness and timing.
//TODO: add shooting
//TODO: give it an ai car and test it
//TODO: ask matt why anyone would want to use quaternions to find the direction the guy is facing.
//TODO: actually think of an ai

bool AiSystem::update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime) {
	bool fire = false;
	if (timer < deltaTime.count()) {
		timer = 0.0;
	} else {
		timer -= deltaTime.count();
	}

	if (state == SIT) {
		fire = sit_behaviour(aiCar, fire);
	}
	else if (state == SPIN) {
		fire = spin_behaviour(aiCar, fire);
	}
	else if (state == MOVETO) {
		fire = moveto_behaviour(aiCar, PxVec3(0, 0, 0), fire);
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
		distribution = std::normal_distribution<double>(5.0, 2.0);
		timer = distribution(rand); //generate a random number with normal distribution with mean of 5 and standard deviation of 2
	}
	return fire;
}

bool AiSystem::spin_behaviour(EngineDriveVehicle* aiCar, bool fire) {
	aiCar->mCommandState.steer = 2.5;
	aiCar->mCommandState.throttle = 1; //speeeeeeeeeeeeeeeeeen
	aiCar->mCommandState.brakes[0] = 0.f;
	aiCar->mCommandState.nbBrakes = 0.f;
	if (timer <= 0.0) {
		state = SIT;
		timer = 0.6;
	}
	return fire;
}

bool AiSystem::moveto_behaviour(EngineDriveVehicle* aiCar, PxVec3 goal, bool fire) {

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
		distribution = std::normal_distribution<double>(5.0, 2.0);
		timer = distribution(rand); //generate a random number with normal distribution with mean of 5 and standard deviation of 2
	}
	else {
		if (angle <= M_PI / 8 && angle >= -M_PI / 8) {
			aiCar->mCommandState.steer = -angle;
		}
		else if (angle < -M_PI / 8) {
			aiCar->mCommandState.steer = 1;
		}
		else if (angle > M_PI / 8) {
			aiCar->mCommandState.steer = -1;
		}
		aiCar->mCommandState.nbBrakes = 0.0f;
		aiCar->mCommandState.brakes[0] = 0.0f;
		aiCar->mCommandState.throttle = 1;
	}
	return fire;
}