#include "SharedDataSystem.h"


/*
* PRIVATE FUNCTIONS
*/

PxVec2 SharedDataSystem::ConvertVec3ToVec2(PxVec3 vectorToConvert) {
	return PxVec2(vectorToConvert.x, vectorToConvert.z);
}

bool SharedDataSystem::IsPointInSquare(PxVec2 point, MapSquare& square) {
	//check if the point is between the two other points. on the line counts
	if (point.x > square.bottomLeft.x && point.x < square.topRight.x &&
		point.y > square.bottomLeft.y && point.y < square.topRight.y) {
		return true;
	}
	return false;
}

bool SharedDataSystem::IsPointInBounds(PxVec2 point) {

	MapSquare boundSquare;
	boundSquare.bottomLeft = BOTTOM_LEFT_MAP_COORD;
	boundSquare.topRight = TOP_RIGHT_MAP_COORD;

	IsPointInSquare(point, boundSquare);

	return IsPointInSquare(point, boundSquare);
}

float SharedDataSystem::DistanceBetweenPoints(const PxVec2& point1, const PxVec2& point2) {
	return (point1 - point2).magnitude();
}

std::vector<PxVec2> SharedDataSystem::GetXNearestPoints(std::vector<PxVec2> pointList, int numPointsToGet, std::vector<PxVec2> pointsOfSameType) {

	//CHATGPT CODE
	std::priority_queue<std::pair<float, PxVec2>, std::vector<std::pair<float, PxVec2>>, CompareDistance> minHeap;

	for (const auto& point : pointsOfSameType) {
		float distance = 0.0f;
		for (const auto& p : pointList) {
			distance += DistanceBetweenPoints(point, p);
		}
		minHeap.push({ distance, point });
		if (minHeap.size() > numPointsToGet) {
			minHeap.pop();
		}
	}

	std::vector<PxVec2> result;
	while (!minHeap.empty()) {
		result.push_back(minHeap.top().second);
		minHeap.pop();
	}

	return result;
}

PxVec2 SharedDataSystem::FindCenterOfFourPointsWithRandomOffset(PxReal minDistance, std::vector<PxVec2> existingPointsList, std::vector<PxVec2> generatedPointsList) {

	//makes one combined vector out of two given in (can have diff number of elements in them)
	std::vector<PxVec2> pointsList;
	for (int i = 0; i < existingPointsList.size(); i++) {

		pointsList.emplace_back(existingPointsList[i]);
	}
	for (int i = 0; i < generatedPointsList.size(); i++) {

		pointsList.emplace_back(generatedPointsList[i]);
	}

	//calculating the random offset
	float randomOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * minDistance - minDistance / 2.0;

	float centerX = (pointsList[0].x + pointsList[1].x + pointsList[2].x + pointsList[3].x) / 4.0f;
	float centerY = (pointsList[0].y + pointsList[1].y + pointsList[2].y + pointsList[3].y) / 4.0f;

	return PxVec2(centerX, centerY);
}

void SharedDataSystem::PopulateMapSquareList(std::vector<PxVec2> pointsOfSameType, std::vector<MapSquare>& mapSquareList) {

	//go through the map square list and populate each one fully before moving on to the next one
	while (pointsOfSameType.size() > 0) {
		for (int i = 0; i < mapSquareList.size(); i++) {
			//gets rid of out of bounds points
			IsPointInBounds(pointsOfSameType.at(0));
			if (!IsPointInBounds(pointsOfSameType.at(0))) {
				pointsOfSameType.erase(pointsOfSameType.begin());
				break;
			}

			//the if statement is what crashes
			if (IsPointInSquare(pointsOfSameType.at(0), mapSquareList[i])) {
				mapSquareList[i].numPoints++;
				mapSquareList[i].pointsInIt.emplace_back(pointsOfSameType.at(0));
				pointsOfSameType.erase(pointsOfSameType.begin());
				break;
			}

		}
	}

}

void SharedDataSystem::RandomizeMapSquareList(std::vector<MapSquare>& mapSquareList) {

	//make a random seed based on the current time
	srand(time(0));

	for (int i = mapSquareList.size() - 1; i > 0; --i) {
		// Generate a random index between 0 and i (inclusive)
		int randomIndex = rand() % (i + 1);

		// Swap the elements at randomIndex and i
		std::swap(mapSquareList[i], mapSquareList[randomIndex]);
	}
}

PxVec3 SharedDataSystem::GenerateSpawnPoint(std::vector<PxVec2> pointsOfSameType, PxReal minDistance, PxReal spawnHeight) {

	PxVec2 spawnPoint;
	std::vector<MapSquare> mapSquareList;

	int xMapSquares = MAPLENGTHX / minDistance;
	int zMapSquares = MAPLENGTHZ / minDistance;

	//divide the map into squares using the minDistance
	for (int i = 0; i < xMapSquares; i++) {
		for (int j = 0; j < zMapSquares; j++) {
			MapSquare square;
			square.id = i * zMapSquares + j;
			square.bottomLeft = PxVec2(BOTTOM_LEFT_MAP_COORD.x + i * minDistance, BOTTOM_LEFT_MAP_COORD.y + j * minDistance);
			square.topRight = PxVec2(BOTTOM_LEFT_MAP_COORD.x + (i + 1) * minDistance, BOTTOM_LEFT_MAP_COORD.y + (j + 1) * minDistance);
			mapSquareList.emplace_back(square);

			MAKE_BOX_DEBUG(BOTTOM_LEFT_MAP_COORD.x + i * minDistance, BOTTOM_LEFT_MAP_COORD.y + j * minDistance);
			MAKE_BOX_DEBUG(BOTTOM_LEFT_MAP_COORD.x + (i + 1) * minDistance, BOTTOM_LEFT_MAP_COORD.y + (j + 1) * minDistance);
		}
	}

	//then make one more row of map squares to cover the last area of the map (in case it doesnt divide nicely)
		//need to get the leftover bit and divide it as much as possible into good ish squares and have one bs square in the top right corner

	//the remaining distance to be split
	float remainingX = MAPLENGTHX - xMapSquares * minDistance;
	float remainingZ = MAPLENGTHZ - zMapSquares * minDistance;

	//add semi nice squares in the z direction
	if (remainingX > 0) {

		//go from x = bottom left map x in increments of minDistance until hit either top right or just less than
		//constant z size

		//loops "up" in the z-direction
		for (int i = BOTTOM_LEFT_MAP_COORD.y; i < TOP_RIGHT_MAP_COORD.y - minDistance; i += minDistance) {

			MapSquare square;
			square.id = mapSquareList.size() + 1;
			square.bottomLeft = PxVec2(TOP_RIGHT_MAP_COORD.x - remainingX, i);
			square.topRight = PxVec2(TOP_RIGHT_MAP_COORD.x, i + minDistance);
			mapSquareList.emplace_back(square);

			MAKE_BOX_DEBUG(TOP_RIGHT_MAP_COORD.x - remainingX, i);
			MAKE_BOX_DEBUG(TOP_RIGHT_MAP_COORD.x, i + minDistance);
		}
	}

	//add semi nice squares in the x direction
	if (remainingZ > 0) {

		//go from z = bottom left map z in increments of minDistance until hit either top right or just less than
		//constant x size

		//loops "right" in the x-direction
		for (int i = BOTTOM_LEFT_MAP_COORD.x; i < TOP_RIGHT_MAP_COORD.x - minDistance; i += minDistance) {

			MapSquare square;
			square.id = mapSquareList.size() + 1;
			square.bottomLeft = PxVec2(i, TOP_RIGHT_MAP_COORD.y - remainingZ);
			square.topRight = PxVec2(i + minDistance, TOP_RIGHT_MAP_COORD.y);
			mapSquareList.emplace_back(square);

			MAKE_BOX_DEBUG(i, TOP_RIGHT_MAP_COORD.y - remainingZ);
			MAKE_BOX_DEBUG(i + minDistance, TOP_RIGHT_MAP_COORD.y);
		}

	}

	//add the corner fucked square
	MapSquare square;
	square.id = mapSquareList.size() + 1;
	square.bottomLeft = PxVec2(TOP_RIGHT_MAP_COORD.x - remainingX, TOP_RIGHT_MAP_COORD.y - remainingZ);
	square.topRight = TOP_RIGHT_MAP_COORD;
	mapSquareList.emplace_back(square);

	MAKE_BOX_DEBUG(TOP_RIGHT_MAP_COORD.x - remainingX, TOP_RIGHT_MAP_COORD.y - remainingZ);
	MAKE_BOX_DEBUG(TOP_RIGHT_MAP_COORD.x, TOP_RIGHT_MAP_COORD.y);

	//place the points in their respecitve squares
	PopulateMapSquareList(pointsOfSameType, mapSquareList);

	//randomize the list to prevent spawning in the same place always
	RandomizeMapSquareList(mapSquareList);

	//find the square with the least amount of points in it
	//if the square has no points in it, find the center and return that
	MapSquare* bestSquare = &mapSquareList[0];

	for (int i = 0; i < mapSquareList.size(); i++) {
		if (mapSquareList[i].numPoints == 0 || mapSquareList[i].numPoints < bestSquare->numPoints) {
			bestSquare = &mapSquareList[i];
			if (bestSquare->numPoints == 0) {

				//random offset
				float randomOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * minDistance - minDistance / 2.0;

				//setting the coords
				spawnPoint.x = ((bestSquare->topRight.x + bestSquare->bottomLeft.x) / 2) + (randomOffset);
				spawnPoint.y = ((bestSquare->topRight.y + bestSquare->bottomLeft.y) / 2) + (randomOffset);
				return PxVec3(spawnPoint.x, spawnHeight, spawnPoint.y);
			}
		}
	}

	boxesMade = true;

	printf("best square: %d\n", bestSquare->numPoints);

	switch (bestSquare->numPoints) {
	case 1:
		//need to find 3 nearest points
		spawnPoint = FindCenterOfFourPointsWithRandomOffset(minDistance, GetXNearestPoints(bestSquare->pointsInIt, 3, pointsOfSameType), bestSquare->pointsInIt);
		break;
	case 2:
		//need to find 2 nearest points
		spawnPoint = FindCenterOfFourPointsWithRandomOffset(minDistance, GetXNearestPoints(bestSquare->pointsInIt, 2, pointsOfSameType), bestSquare->pointsInIt);
		break;
	case 3:
		//need to find nearest point
		spawnPoint = FindCenterOfFourPointsWithRandomOffset(minDistance, GetXNearestPoints(bestSquare->pointsInIt, 1, pointsOfSameType), bestSquare->pointsInIt);
		break;
	case 4:
		//make square, return middle
		spawnPoint = FindCenterOfFourPointsWithRandomOffset(minDistance, bestSquare->pointsInIt);
		break;
	default:
		//make a random point in the square and return that
		spawnPoint.x = std::rand() / static_cast<double>(RAND_MAX) * MAPLENGTHX;
		spawnPoint.y = std::rand() / static_cast<double>(RAND_MAX) * MAPLENGTHZ;
		break;
	}

	return PxVec3(spawnPoint.x, spawnHeight, spawnPoint.y);
}

/*
* PUBLIC FUNCTIONS
*/

void SharedDataSystem::MAKE_BOX_DEBUG(PxReal x, PxReal z) {

	if (!boxesMade) {

		//define a projectile
		physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(0.25f, 0.25f, 0.25f), *gMaterial);

		PxRigidStatic* projectileBody = gPhysics->createRigidStatic(PxTransform(x, 5, z));
		projectileBody->attachShape(*shape);
		projectileBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);
		gScene->addActor(*projectileBody);
	}
}

CarInfo* SharedDataSystem::GetCarInfoStructFromEntity(std::shared_ptr<Entity> entity) {

	for (int i = 0; i < carInfoList.size(); i++) {
		if (carInfoList[i].entity->name == entity->name) {
			return &carInfoList[i];
		}
	}

	//unreachable code
	printf("Failed to Get Car Info Struct From Entity");
	exit(69);
}

EngineDriveVehicle* SharedDataSystem::GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic) {

	for (int i = 0; i < carRigidDynamicList.size(); i++) {

		if (carRigidDynamicList[i] == carRigidDynamic) {
			return gVehicleList[i];
		}
	}

	//unreachable code
	printf("Failed to Get Vehicle From Rigid Dynamic List.");
	exit(69);
}

PxRigidDynamic* SharedDataSystem::GetRigidDynamicFromVehicle(EngineDriveVehicle* gVehicle) {

	for (int i = 0; i < gVehicleList.size(); i++) {

		if (gVehicleList[i] == gVehicle) {
			return carRigidDynamicList[i];
		}
	}

	//unreachable code
	printf("Failed to Get Rigid Dynamic From Vehicle.");
	exit(69);
}

std::shared_ptr<Entity> SharedDataSystem::GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic) {

	for (int i = 0; i < entityList.size(); i++) {

		if (entityList.at(i).collisionBox == rigidDynamic) {
			return std::make_shared<Entity>(entityList[i]);
		}
	}

	//unreachable code
	printf("Failed to Get Entity From Rigid Dynamic List.");
	exit(69);
}

std::vector<CarInfo*> SharedDataSystem::GetListOfDeadCars() {

	std::vector<CarInfo*> deadCarVec;

	for (int i = 0; i < carInfoList.size(); i++) {
		if (carInfoList[i].isAlive == false) {
			deadCarVec.emplace_back(&carInfoList[i]);
		}
	}

	return deadCarVec;
}

PxVec3 SharedDataSystem::DetermineRespawnLocation(PhysicsType physType) {

	std::vector<PxVec2> locations;

	//uses physics type to determine min spacing
	switch (physType) {
	case PhysicsType::CAR:

		for (int i = 0; i < carInfoList.size(); i++) {

			//dont care about location of dead cars
			if (carInfoList[i].isAlive) {
				locations.emplace_back(carInfoList[i].entity->collisionBox->getGlobalPose().p.x, carInfoList[i].entity->collisionBox->getGlobalPose().p.z);
			}
		}
		return GenerateSpawnPoint(locations, CAR_MIN_SPAWN_DISTANCE, CAR_SPAWN_HEIGHT);

		break;
	case PhysicsType::POWERUP:

		for (int i = 0; i < allPowerupList.size(); i++) {
			locations.emplace_back(PxVec2(allPowerupList[i].entity->collisionBox->getGlobalPose().p.x, allPowerupList[i].entity->collisionBox->getGlobalPose().p.z));
		}
		return GenerateSpawnPoint(locations, POWERUP_MIN_SPAWN_DISTANCE, POWERUP_SPAWN_HEIGHT);

		break;
	default:
		printf("this is a physics type that cannot be respawned");
		break;
	}

	//unreachable code
	printf("Failed to Determine Spawn Location.");
	exit(69);
}

std::shared_ptr<Entity> SharedDataSystem::GetCarThatShotProjectile(PxRigidDynamic* projectile) {

	//not sure if there is a better way to do this
	//go through every list of shot projectiles and search for the passed one
	//return the car
	for (auto iterator = carProjectileRigidDynamicDict.begin(); iterator != carProjectileRigidDynamicDict.end(); ++iterator) {

		for (PxRigidDynamic* listProjectile : iterator->second) {
			if (listProjectile == projectile) {
				if (DEBUG_MODE) printf("GetCarThatShotProjectile hard debug\n");
				return GetEntityFromRigidDynamic(iterator->first);
			}
		}
	}

	//unreachable code
	printf("Failed Get Car That Shot Projectile.");
	exit(69);

}

PowerupInfo* SharedDataSystem::GetPowerupInfoStructFromEntity(std::shared_ptr<Entity> entity) {

	for (int i = 0; i < allPowerupList.size(); i++) {
		if (allPowerupList[i].entity->name == entity->name) {
			return &allPowerupList[i];
		}
	}

	//unreachable code
	exit(69);
}

void SharedDataSystem::CarProjectileCollisionLogic(PxActor* car, PxActor* projectile) {

	if (DEBUG_MODE) printf("CarProjectileCollisionLogic before\n");

	std::shared_ptr<Entity> carEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);

	if (DEBUG_MODE) printf("CarProjectileCollisionLogic after\n");

	//increase score of car that shot
	CarInfo* shootingCarInfo = GetCarInfoStructFromEntity(GetCarThatShotProjectile((PxRigidDynamic*)projectile));
	shootingCarInfo->score++;

	/*
	* remove the projectile from all lists
	*/

	//entity list
	for (int i = 0; i < entityList.size(); i++) {
		if (entityList[i].name == projectileEntity->name) {
			entityList.erase(entityList.begin() + i);
		}
	}

	//car projectile dict
	for (int i = 0; i < carProjectileRigidDynamicDict[(PxRigidDynamic*)car].size(); i++) {
		if (carProjectileRigidDynamicDict[(PxRigidDynamic*)car][i] == (PxRigidDynamic*)projectile) {
			carProjectileRigidDynamicDict[(PxRigidDynamic*)car].erase(carProjectileRigidDynamicDict[(PxRigidDynamic*)car].begin() + i);
		}
	}

	//delete the projectile
	gScene->removeActor(*projectile);
	projectile->release();

	//make a sound
	SoundsToPlay.push_back(std::make_pair(std::string("Bwud"), getSoundRotMat() * carEntity->collisionBox->getGlobalPose().p));

	//setting the data of the car that got hit to let it respawn
	CarInfo* hitCar = GetCarInfoStructFromEntity(carEntity);
	hitCar->respawnTimeLeft = CAR_RESPAWN_LENGTH;
	hitCar->isAlive = false;
	//moving into the sky and disabling gravity to "delete it"
	hitCar->entity->collisionBox->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);
	PxReal yShift = hitCar->entity->collisionBox->getGlobalPose().p.y + 150;
	PxVec3 carShift(hitCar->entity->collisionBox->getGlobalPose().p.x, yShift, hitCar->entity->collisionBox->getGlobalPose().p.z);
	hitCar->entity->collisionBox->setGlobalPose(PxTransform(carShift));

}

void SharedDataSystem::CarPowerupCollisionLogic(PxActor* car, PxActor* powerup) {

	if (DEBUG_MODE) printf("CarPowerupCollisionLogic before\n");

	//converting the actors to entities
	std::shared_ptr<Entity> carEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	std::shared_ptr<Entity> powerupEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)powerup);

	if (DEBUG_MODE) printf("CarPowerupCollisionLogic after\n");

	//gives the car the powerups effect
	switch (GetPowerupInfoStructFromEntity(powerupEntity)->powerupType) {
	case PowerupType::AMMO:

		GetCarInfoStructFromEntity(carEntity)->ammoCount += NUMBER_AMMO_GIVEN_PER_POWERUP;
		break;
	case PowerupType::CARSPEED:

		break;
	case PowerupType::PROJECTILESIZE:

		break;
	case PowerupType::PROJECTILESPEED:

		break;
	default:
		printf("unknown powerup type\n");
		break;
	}

	/*
	* kill the powerup (remove from all lists and scene)
	*/

	//all powerup list
	for (int i = 0; i < allPowerupList.size(); i++) {
		if (allPowerupList[i].entity->name == powerupEntity->name) {
			allPowerupList.erase(allPowerupList.begin() + i);
		}
	}

	//entity list
	for (int i = 0; i < entityList.size(); i++) {
		if (entityList[i].name == powerupEntity->name) {
			entityList.erase(entityList.begin() + i);
		}
	}

	//scene
	gScene->removeActor(*powerupEntity->collisionBox);
	powerupEntity->collisionBox->release();

}

void SharedDataSystem::ProjectileStaticCollisionLogic(PxActor* projectile) {

	if (DEBUG_MODE) printf("ProjectileStaticCollisionLogic before\n");

	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);
	PxRigidDynamic* carThatShotProjectile = GetCarThatShotProjectile((PxRigidDynamic*)projectile)->collisionBox;

	if (DEBUG_MODE) printf("ProjectileStaticCollisionLogic after\n");

	/*
	* remove the projectile from all lists
	*/

	//entity list
	for (int i = 0; i < entityList.size(); i++) {
		if (entityList[i].name == projectileEntity->name) {
			entityList.erase(entityList.begin() + i);
		}
	}

	//car projectile dict
	for (int i = 0; i < carProjectileRigidDynamicDict[carThatShotProjectile].size(); i++) {
		if (carProjectileRigidDynamicDict[carThatShotProjectile][i] == (PxRigidDynamic*)projectile) {
			carProjectileRigidDynamicDict[carThatShotProjectile].erase(carProjectileRigidDynamicDict[carThatShotProjectile].begin() + i);
		}
	}

	//delete the projectile
	gScene->removeActor(*projectile);
	projectile->release();
}

void SharedDataSystem::ResolveCollisions() {

	//if a collision has occured
	if (gContactReportCallback->contactDetected) {

		//code readability variables
		PxActor* actor1 = gContactReportCallback->contactPair.actors[0];
		PxActor* actor2 = gContactReportCallback->contactPair.actors[1];

		if (DEBUG_MODE) printf("ResolveCollisions before\n");

		//get the two entities that collided
		std::shared_ptr<Entity> entity1 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor1);
		std::shared_ptr<Entity> entity2 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor2);

		if (DEBUG_MODE) printf("ResolveCollisions after\n");

		//determines the logic to use
		switch (entity1->physType) {
		case PhysicsType::CAR:

			switch (entity2->physType) {
			case PhysicsType::PROJECTILE:
				CarProjectileCollisionLogic(actor1, actor2);
				break;
			case PhysicsType::POWERUP:
				CarPowerupCollisionLogic(actor1, actor2);
				break;
			default:
				break;
			}

			break;
		case PhysicsType::PROJECTILE:

			switch (entity2->physType) {
			case PhysicsType::CAR:
				CarProjectileCollisionLogic(actor2, actor1);
				break;
			case PhysicsType::STATIC:
				ProjectileStaticCollisionLogic(actor1);
				break;
			default:
				break;
			}

			break;
		case PhysicsType::STATIC:

			if (entity2->physType == PhysicsType::PROJECTILE) {
				ProjectileStaticCollisionLogic(actor2);
			}

			break;
		case PhysicsType::POWERUP:

			if (entity2->physType == PhysicsType::CAR) {
				CarPowerupCollisionLogic(actor2, actor1);
			}

			break;
		default:
			printf("unknown physics type of colliding object\n");
			break;
		}

	}

	//resolved the collision
	gContactReportCallback->contactDetected = false;
}

void SharedDataSystem::resetSharedDataSystem() {
	// Clear all lists
	carProjectileRigidDynamicDict.clear();
	entityList.clear();
	carRigidDynamicList.clear();
	gVehicleList.clear();
	carInfoList.clear();
	allPowerupList.clear();

	//reset variables
	spawnedPowerupCounter = 0;

	//add the map to the entity list
	entityList.emplace_back(MAP);
}

void SharedDataSystem::menuEventHandler() {
	// Only handle events when in menu
	if (inMenu) {
		menuOptionIndex = 0;
	}
}

glm::mat3 SharedDataSystem::getCamRotMat() {
	return glm::mat3({ cos(cameraAngle), 0, sin(cameraAngle) }, { 0, 1, 0 }, { -sin(cameraAngle), 0, -cos(cameraAngle) });
}

PxMat33 SharedDataSystem::getCamRotMatPx(float angle) {
	return PxMat33({ cos(angle), 0, sin(angle) }, { 0, 1, 0 }, { -sin(angle), 0, -cos(angle) });
}

PxMat33 SharedDataSystem::getSoundRotMat() {
	return PxMat33({ cos((float)M_PI - cameraAngle), 0, sin((float)M_PI - cameraAngle) }, { 0, 1, 0 }, { -sin((float)M_PI - cameraAngle), 0, -cos((float)M_PI - cameraAngle) });
}