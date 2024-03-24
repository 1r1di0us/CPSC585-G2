#include "AiSystem.h"

AiSystem::AiSystem() {} //empty constructor to keep a global scope on AiSystems


AiSystem::AiSystem(SharedDataSystem* dataSys, EngineDriveVehicle* aiCar) {
	this->dataSys = dataSys;
	this->aiCar = aiCar;
	state = STATE::hunting;
	// state = 0; // hunting = 0 hiding = 1
	startTimer = 1.5;
	brakeTimer = 0;
	coolDownTimer = 0;
	reverseTimer = 0;
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

	edgeNodes.emplace_back(navMesh->nodes->at(62)); //[2][2]
	edgeNodes.emplace_back(navMesh->nodes->at(165)); //[5][15]
	edgeNodes.emplace_back(navMesh->nodes->at(87)); //[2][27]
	edgeNodes.emplace_back(navMesh->nodes->at(444)); //[14][24]
	edgeNodes.emplace_back(navMesh->nodes->at(837)); //[27][27]
	edgeNodes.emplace_back(navMesh->nodes->at(734)); //[24][14]
	edgeNodes.emplace_back(navMesh->nodes->at(812)); //[27][2]
	edgeNodes.emplace_back(navMesh->nodes->at(455)); //[15][5]
}

bool AiSystem::update(std::chrono::duration<double> deltaTime) {
	aiCarInfo = dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(dataSys->GetRigidDynamicFromVehicle(aiCar)));
	carPos = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p;
	carPos.y = 0;
	// decrement timers
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
	if (reverseTimer < deltaTime.count() && reverseTimer > 0) { //positive reverse timer is the time until we decide to reverse
		reverseTimer = -2.5; //negative reverse timer is the time we actually reverse
		aiCar->mTransmissionCommandState.targetGear = 0;
	}
	else if (reverseTimer > deltaTime.count()) {
		reverseTimer -= deltaTime.count();
	}
	else if (reverseTimer > -deltaTime.count() && reverseTimer < 0) {
		reverseTimer = 0.0;
		aiCar->mTransmissionCommandState.targetGear = 2; //set back in first gear
	}
	else if (reverseTimer < -deltaTime.count()) {
		reverseTimer += deltaTime.count();
	}

	bool fire = false;

	if (state == STATE::hunting) fire = hunting_behaviour(fire); //hunting first because why not
	if (state == STATE::hiding) fire = hiding_behaviour(fire);

	move_car();
	aim_car(deltaTime);
	
	return fire;
} 

void AiSystem::astar_path_finding() {
	//no idea what I'm doing :)
	Node* carNode = navMesh->findEntity(carPos);
	if (pathFinder->search(carNode, moveNode) == false) {
		std::cout << "PATHFINDING FAILURE!" << std::endl;
	}
	else {
		moveLocation = pathFinder->getNextWaypoint();
	}
	//degreeOfFreedom = ceil((int)pathFinder->path.size() / 2) + 1; // maximum degrees of freedom
}

void AiSystem::move_car() {
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

	if (pathFinder->path.size() < 1) {
		moveNode = nullptr; //tell the ai to find a new path
		aiCar->mCommandState.throttle = 0; //stand still while we do
		aiCar->mCommandState.nbBrakes = 0;
		aiCar->mCommandState.brakes[0] = 0;
	}
	else {
		int freedom = 10;//abs(degreeOfFreedom - (int)pathFinder->path.size()) * 5 + 5;

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
	
	if (reverseTimer < 0) { //negative timer means we reverse
		aiCar->mCommandState.steer = 0;
		aiCar->mCommandState.throttle = 1; //transmission is already done in update()
		aiCar->mCommandState.nbBrakes = 0;
		aiCar->mCommandState.brakes[0] = 0;
	}

	if (aiCar->mCommandState.throttle == 1 && carSpeed < 1 && reverseTimer != 0) {
		reverseTimer = 2.5;
	}
	else if (aiCar->mCommandState.throttle == 1 && reverseTimer > 0 && carSpeed > 1) {
		reverseTimer = 0;
		//we moving again
	}
}

void AiSystem::aim_car(std::chrono::duration<double> deltaTime) {
	if (target != nullptr) {
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
	if (angle > 0.5 * deltaTime.count()) { //enemies rotate turrets much slower
		aiCarInfo->shootDir = dataSys->getRotMatPx((float)(-0.75 * deltaTime.count())) * aiCarInfo->shootDir;
	}
	else if (angle < -0.5 * deltaTime.count()) {
		aiCarInfo->shootDir = dataSys->getRotMatPx((float)(0.75 * deltaTime.count())) * aiCarInfo->shootDir;
	}
	else {
		aiCarInfo->shootDir = dataSys->getRotMatPx(-angle) * aiCarInfo->shootDir;
	}
	
	if (abs(angle) < 0.1) { //start bad
		lockedOn = true;
	}
}

bool AiSystem::hunting_behaviour(bool fire) {
	if (aiCarInfo->ammoCount <= 1) { //transition out of this
		state = STATE::hiding;
		moveNode = nullptr;
		transitioning == true;
		return fire;
	}

	// for when coming out of hiding, not sure what we would do
	if (transitioning) {
		transitioning = false;
		nodeIterator = 8; //reshuffle centerNodes
	}

	//patrol center
	if (moveNode == nullptr || moveLocation == PxVec3(-100, -100, -100)) { //if we need to pathfind
		// iterate through a randomized list of preset nodes
		nodeIterator++;
		if (nodeIterator == centerNodes.size()) { //once we have gone through all the preset nodes, randomize the list again and start over
			nodeIterator = 0;
			std::random_device rand;
			std::mt19937 gen(rand());

			std::shuffle(centerNodes.begin(), centerNodes.end(), gen); //shuffle nodes in centerNodes
		}
		moveNode = centerNodes[nodeIterator];
		astar_path_finding();
	}

	//untarget enemies that are too far away
	if (target != nullptr && target->entity != nullptr) {
		if (!target->isAlive) { //don't target things that are dead
			target = nullptr;
		}
		else if ((target->entity->collisionBox->getGlobalPose().p - carPos).magnitude() > 75.0) {
			target = nullptr;
		}
		//need to check if they are behind walls
	}
	
	//get new target
	if (target == nullptr) {
		enemyDist = 150;
		for (int i = 0; i < dataSys->carInfoList.size(); i++) {
			if (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p != carPos) { // if the car is not us
				if ((dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - carPos).magnitude() < enemyDist) {
					//need to also check if they are behind walls
					enemyDist = (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - carPos).magnitude();
					target = &dataSys->carInfoList[i];
					//get direction to target
					aimDir = (target->entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).getNormalized();
					//add random aim
					std::default_random_engine generator;
					std::normal_distribution<float> distribution(0, 0.5); //mean of 0, standard deviation of 0.5 (pi/8 ~0.4)
					float rand = distribution(generator);
					aimDir = dataSys->getRotMatPx(rand) * aimDir;
				}
			}
		}
	}

	if (target == nullptr) {
		aimDir = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2(); //no valid targets
		std::cout << "NO VALID TARGETS" << std::endl;
	}
	else {
		enemyDist = (target->entity->collisionBox->getGlobalPose().p - carPos).magnitude();
		if (enemyDist > 0 && enemyDist < 20) { // watch out this overrides startTimer
			//enemy very close
			if (lockOnTime > 0.1 && !wantToFire && coolDownTimer == 0) { //we are locked on but don't want to fire yet
				brakeTimer = 0.5; // speedy fire
				wantToFire = true;
			}
			else if (lockOnTime == 0 && wantToFire) { //we want to fire but we aren't locked on anymore
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
		else if (startTimer == 0) {
			if (lockOnTime > 0.5 && !wantToFire && coolDownTimer == 0) { //we are locked on but don't want to fire yet
				brakeTimer = 1;
				wantToFire = true;
			}
			else if (lockOnTime == 0 && wantToFire) {  //we want to fire but we aren't locked on anymore
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
	}

	return fire;
}

bool AiSystem::hiding_behaviour(bool fire) {

	if (aiCarInfo->ammoCount > 1) { //first, check if we need to transition to hunting
		state = STATE::hunting;
		moveNode = nullptr;
		transitioning == true;
		wantToFire = false;
		return fire;
	}

	if (transitioning) {
		transitioning = false;
		float dist = 150;
		for (int i = 0; i < edgeNodes.size(); i++) { //find closest preset edgeNode
			if ((edgeNodes[i]->centroid - carPos).magnitude() < dist) {
				dist = (edgeNodes[i]->centroid - carPos).magnitude();
				nodeIterator = i;
			}
		}
	}

	//If the powerup has been stolen, targetPowerup should automatically be a null ptr.
	if (movingToPowerup && targetPowerup == nullptr) { //first, if powerup we want has been stolen stop moving there
		moveNode = nullptr;
		movingToPowerup = false;
	}

	//if target is too close
	/*if ((target->entity.get()->collisionBox->getGlobalPose().p - carPos).magnitude() < 30) {
		if (aiCarInfo->ammoCount == 1) {
			wantToFire = true; //fire when locked on
		}
		int nextNode;
		int prevNode;
		if (nodeIterator + 1 == (int)edgeNodes.size()) nextNode = 0;
		else nextNode = nodeIterator + 1;
		if (nodeIterator == 0) prevNode = edgeNodes.size();
		else prevNode = nodeIterator - 1;
		if ((edgeNodes[nextNode]->centroid - (carPos + aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2())).magnitude() <=
			(edgeNodes[prevNode]->centroid - (carPos + aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2())).magnitude()) {
			//if we are facing and/or closer to the next Node, set that as the moveNode
			moveNode = edgeNodes[nextNode];
			nodeIterator = nextNode;
		}
		else { //otherwise move to the previous node
			moveNode = edgeNodes[prevNode];
			nodeIterator = prevNode;
		}
		astar_path_finding();
	}*/

	if (moveNode == nullptr || moveLocation == PxVec3(-100, -100, -100)) { // if we aren't moving to a location
		float powerupDist = 150;
		for (int i = 0; i < dataSys->allPowerupList.size(); i++) { //check for a nearby powerup to pick up. We could do this every time this is called but lets not.
			float dist = (dataSys->allPowerupList[i].entity->collisionBox->getGlobalPose().p - carPos).magnitude();
			if (dist < powerupDist && dist < 40) {
				powerupDist = dist;
				moveNode == navMesh->findEntity(dataSys->allPowerupList[i].entity->collisionBox->getGlobalPose().p);
				targetPowerup = dataSys->allPowerupList[i].entity->collisionBox;
				movingToPowerup = true;
			}
		}

		if (moveNode == nullptr) { //we didn't find a powerup to get
			//patrol edges
			int nextNode;
			int prevNode;
			if (nodeIterator + 1 == (int)edgeNodes.size()) nextNode = 0;
			else nextNode = nodeIterator + 1;
			if (nodeIterator == 0) prevNode = edgeNodes.size();
			else prevNode = nodeIterator - 1;
			if ((edgeNodes[nextNode]->centroid - (carPos + aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2())).magnitude() <=
				(edgeNodes[prevNode]->centroid - (carPos + aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2())).magnitude()) {
				//if we are facing and/or closer to the next Node, set that as the moveNode
				moveNode = edgeNodes[nextNode];
				nodeIterator = nextNode;
			}
			else { //otherwise move to the previous node
				moveNode = edgeNodes[prevNode];
				nodeIterator = prevNode;
			}
		}
		astar_path_finding(); //we always find a node either way
	}

	//get new target
	if (target == nullptr) {
		enemyDist = 150;
		for (int i = 0; i < dataSys->carInfoList.size(); i++) {
			if (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p != aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p) {
				if ((dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude() < enemyDist) {
					//need to also check if they are behind walls
					enemyDist = (dataSys->carInfoList[i].entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude();
					target = &dataSys->carInfoList[i];
					//get direction to target
					aimDir = (target->entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).getNormalized();
					//add random aim
					std::default_random_engine generator;
					std::normal_distribution<float> distribution(0, 0.5); //mean of 0, standard deviation of 0.5 (pi/8 ~0.4)
					float rand = distribution(generator);
					aimDir = dataSys->getRotMatPx(rand) * aimDir;
				}
			}
		}
	}

	if (target == nullptr) {
		aimDir = aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2(); //no valid targets
	}
	else {
		enemyDist = (target->entity->collisionBox->getGlobalPose().p - aiCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p).magnitude();
		if (startTimer == 0) {
			if (wantToFire && lockOnTime >= 0.25) { //no slowing down this time
				fire = true; //FIRE IN THE HOLE!!!
				std::cout << "FIRE IN THE HOLE!!!" << std::endl;
				wantToFire = false;
				lockOnTime = 0; //make sure it doesn't fire again for a bit
				coolDownTimer = 1.0;
			}
		}
	}

	return fire;
}