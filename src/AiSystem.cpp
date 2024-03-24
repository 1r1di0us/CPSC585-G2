#include "AiSystem.h"

AiSystem::AiSystem(SharedDataSystem* dataSys) {
	this->dataSys = dataSys;
	startTimer = 1.5;
	brakeTimer = 0;
	coolDownTimer = 0;
	lockOnTime = 0;
	navMesh = new NavMesh();
	pathFinder = new PathFinder(navMesh);
	moveLocation = PxVec3(-100, -100, -100);
	moveNode = nullptr;
	
	centerNodes.emplace_back(navMesh->nodes->at(372)); //[12][12]
	centerNodes.emplace_back(navMesh->nodes->at(522)); //[17][12]
	centerNodes.emplace_back(navMesh->nodes->at(527)); //[17][17]
	centerNodes.emplace_back(navMesh->nodes->at(377)); //[12][17]
	centerNodes.emplace_back(navMesh->nodes->at(314)); //[10][14]
	centerNodes.emplace_back(navMesh->nodes->at(585)); //[19][15]
	centerNodes.emplace_back(navMesh->nodes->at(430)); //[14][10]
	centerNodes.emplace_back(navMesh->nodes->at(469)); //[15][19] nice
}

bool AiSystem::update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime) {
	aiCarInfo = dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(dataSys->GetRigidDynamicFromVehicle(aiCar)));
	if (startTimer < deltaTime.count()) {
		startTimer = 0.0;
	}
	else {
		startTimer -= deltaTime.count();
	}
	if (brakeTimer < deltaTime.count()) {
		brakeTimer = 0.0;
	}
	else {
		brakeTimer -= deltaTime.count();
	}
	if (coolDownTimer < deltaTime.count()) {
		coolDownTimer = 0.0;
	}
	else {
		coolDownTimer -= deltaTime.count();
	}
	bool fire = false;
	//if (aiCarInfo->ammoCount > 1)
	fire = hunting_behaviour(aiCar, fire);
	//else fire = hiding_behaviour(aiCar, fire);
	move_car(aiCar);
	aim_car(aiCar, deltaTime);
	
	return fire;
} 

void AiSystem::astar_path_finding(EngineDriveVehicle* aiCar) {
	//no idea what I'm doing :)
	PxVec3 carPos = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	Node* carNode = navMesh->findEntity(carPos);
	if (pathFinder->search(carNode, moveNode) == false) {
		std::cout << "PATHFINDING FAILURE!" << std::endl;
	}
	else {
		moveLocation = pathFinder->getNextWaypoint();
	}
	//degreeOfFreedom = ceil((int)pathFinder->path.size() / 2) + 1; // maximum degrees of freedom
}

void AiSystem::move_car(EngineDriveVehicle* aiCar) {
	PxVec3 carPos = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	float carSpeed = aiCar->mPhysXState.physxActor.rigidBody->getLinearVelocity().magnitude();
	float dist = (moveLocation - carPos).magnitude();
	PxVec3 intentDir = (moveLocation - carPos).getNormalized();
	float x = intentDir.x;
	intentDir.x = intentDir.z;
	intentDir.z = -x; //rotate it 90 degrees
	// I don't know how to explain why but it has to do with the car being able to turn more than 180 degrees
	PxVec3 carDir = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();

	//get the angle between intentDir and carDir
	float dot = carDir.dot(intentDir);
	float det = PxVec3(0, 1, 0).dot(carDir.cross(intentDir));
	//triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
	float angle = atan2(dot, det);

	int freedom = 5;

	if (pathFinder->path.size() <= 1) {
		moveNode = nullptr; //tell the ai to find a new path
		aiCar->mCommandState.throttle = 0; //stand still while we do
		aiCar->mCommandState.nbBrakes = 0;
		aiCar->mCommandState.brakes[0] = 0;
	}
	else {
		freedom = 10;//abs(degreeOfFreedom - (int)pathFinder->path.size()) * 5 + 5;

		if (dist < freedom) {
			moveLocation = pathFinder->getNextWaypoint();
		}
		else {
			if (angle <= 1 && angle >= -1) {
				aiCar->mCommandState.steer = -angle;
				aiCar->mCommandState.throttle = 1;
				aiCar->mCommandState.nbBrakes = 0;
				aiCar->mCommandState.brakes[0] = 0;
			}
			else if (angle < -1) {
				aiCar->mCommandState.steer = 1;
				if (carSpeed > 15.0) {
					aiCar->mCommandState.throttle = 0;
					aiCar->mCommandState.nbBrakes = 1;
					aiCar->mCommandState.brakes[0] = 1;
				}
				else {
					aiCar->mCommandState.throttle = 1;
					aiCar->mCommandState.nbBrakes = 0;
					aiCar->mCommandState.brakes[0] = 0;
				}
			}
			else if (angle > 1) {
				aiCar->mCommandState.steer = -1;
				if (carSpeed > 15.0) {
					aiCar->mCommandState.throttle = 0;
					aiCar->mCommandState.nbBrakes = 1;
					aiCar->mCommandState.brakes[0] = 1;
				}
				else {
					aiCar->mCommandState.throttle = 1;
					aiCar->mCommandState.nbBrakes = 0;
					aiCar->mCommandState.brakes[0] = 0;
				}
			}

			if (brakeTimer > 0.0) {
				if (carSpeed > 3.0) {
					aiCar->mCommandState.throttle = 0;
					aiCar->mCommandState.nbBrakes = 1;
					aiCar->mCommandState.brakes[0] = 1;
				}
			}
		}
	}

}

void AiSystem::aim_car(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime) {
	PxVec3 aimDir;
	if (target == nullptr) {
		aimDir = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
	}
	else {
		aimDir = (target->entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).getNormalized();
		if (lockedOn) {
			lockOnTime += (double)deltaTime.count();
		}
		else {
			lockOnTime = 0;
		}
	}

	//get angle between shootDir and aimDir
	float dot = aiCarInfo->shootDir.dot(aimDir);
	float det = PxVec3(0, 1, 0).dot(aiCarInfo->shootDir.cross(aimDir));
	//triple product to obtain the determinant of the 3x3 matrix (n, carDir, intentDir)
	float angle = atan2(dot, det);
	//move shootDir slowly
	if (angle > 0.75 * deltaTime.count()) {
		aiCarInfo->shootDir = dataSys->getRotMatPx((float)(-0.75 * deltaTime.count())) * aiCarInfo->shootDir;
	}
	else if (angle < -0.75 * deltaTime.count()) {
		aiCarInfo->shootDir = dataSys->getRotMatPx((float)(0.75 * deltaTime.count())) * aiCarInfo->shootDir;
	}
	else {
		aiCarInfo->shootDir = dataSys->getRotMatPx(-angle) * aiCarInfo->shootDir;
	}
	
	if (abs(angle) < 0.1) { //start bad
		lockedOn = true;
	}
}

bool AiSystem::hunting_behaviour(EngineDriveVehicle* aiCar, bool fire) {
	//patrol center
	if (moveNode == nullptr || moveLocation == PxVec3(-100, -100, -100)) {
		// iterate through a randomized list of preset nodes
		nodeIterator++;
		if (nodeIterator == centerNodes.size()) { //once we have gone through all the preset nodes, randomize the list again and start over
			nodeIterator = 0;
			std::random_device rand;
			std::mt19937 gen(rand());

			std::shuffle(centerNodes.begin(), centerNodes.end(), gen); //shuffle nodes in centerNodes
		}
		moveNode = centerNodes[nodeIterator];
		astar_path_finding(aiCar);
	}

	//untarget enemies that are too far away
	if (target != nullptr && target->entity != nullptr) {
		if (!target->isAlive) { //don't target things that are dead
			target = nullptr;
		}
		else if ((target->entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude() > 75.0) {
			target = nullptr;
		}
		//need to check if they are behind walls
	}

	//get new target
	if (target == nullptr) {
		float enemyDist = 150;
		for (int i = 0; i < dataSys->carInfoList.size(); i++) {
			if (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p != aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p) {
				if ((dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude() < enemyDist) {
					//need to also check if they are behind walls
					enemyDist = (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude();
					target = &dataSys->carInfoList[i];
				}
			}
		}
	}

	if (target != nullptr && startTimer == 0) { //aka no valid targets
		if (lockOnTime > 0.5 && !wantToFire && coolDownTimer == 0) {
			brakeTimer = 1;
			wantToFire = true;
		}
		else if (lockOnTime == 0 && wantToFire) {
			wantToFire = false;
			brakeTimer = 0;
		}
		else if (brakeTimer == 0 && wantToFire) {
			fire = true; //FIRE IN THE HOLE!!!
			std::cout << "FIRE IN THE HOLE!!!" << std::endl;
			wantToFire = false;
			lockOnTime = 0; //make sure it doesn't fire again for a bit
			coolDownTimer = 1.0;
		}
	}

	return fire;
}

bool AiSystem::hiding_behaviour(EngineDriveVehicle* aiCar, bool fire) {
	return fire;
}