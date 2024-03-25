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

	//clear the data from the previous iteration
	for (int i = 0; i < mapSquareList.size(); i++) {

		mapSquareList[i].numPoints = 0;
		mapSquareList[i].pointsInIt.clear();
	}

	//place each point in the correct map square
		//ignores points out of bounds and points that are out of any map squares (deleting them due to obstacles)
	for (int i = 0; i < pointsOfSameType.size(); i++) {

		for (int j = 0; j < mapSquareList.size(); j++) {

			if (IsPointInSquare(pointsOfSameType.at(i), mapSquareList[j])) {
				mapSquareList[j].numPoints++;
				mapSquareList[j].pointsInIt.emplace_back(pointsOfSameType.at(i));
				break;
			}

		}
	}

}

void SharedDataSystem::RandomizeMapSquareList(std::vector<MapSquare>& mapSquareList) {

	for (int i = mapSquareList.size() - 1; i > 0; --i) {
		// Generate a random index between 0 and i (inclusive)
		int randomIndex = rand() % (i + 1);

		// Swap the elements at randomIndex and i
		std::swap(mapSquareList[i], mapSquareList[randomIndex]);
	}
}

bool SharedDataSystem::IsSpawnPointValid(PxVec2 potentialSpawnPoint) {

	//go through all obstacles
	for (int i = 0; i < obstacleMapSquareList.size(); i++) {

		//if the point is inside the square made by the max dimensions of an obstacle
		if (IsPointInSquare(potentialSpawnPoint, obstacleMapSquareList[i])) {
			if (DEBUG_PRINTS) printf("CheckPotentialSpawnPoint spawnpoint is in obstacle\n");
			return false;
		}

	}

	return true;
}

PxVec3 SharedDataSystem::GenerateValidSpawnPoint(std::vector<MapSquare> mapSquareList, std::vector<PxVec2> pointsOfSameType, PxReal minDistance, PxReal spawnHeight) {

	PxVec2 spawnPoint;
	bool foundPoint;
	MapSquare* bestSquare;

	//for the case where it gets stuck infinite looping because the spawn square is invalid and cant choose another one cause random is outside of this loop
	int minAcceptablePoints = 0;
	int maxAcceptableLoops = 10;

	do {

		foundPoint = false;

		//find the square with the least amount of points in it
		//if the square has no points in it, find the center and return that
		bestSquare = &mapSquareList[0];

		for (int i = 0; i < mapSquareList.size(); i++) {
			if (mapSquareList[i].numPoints == minAcceptablePoints || mapSquareList[i].numPoints < bestSquare->numPoints) {
				bestSquare = &mapSquareList[i];
				if (bestSquare->numPoints == 0) {

					//random offset
					float randomOffset = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * minDistance - minDistance / 2.0;

					//setting the coords
					spawnPoint.x = ((bestSquare->topRight.x + bestSquare->bottomLeft.x) / 2) + (randomOffset);
					spawnPoint.y = ((bestSquare->topRight.y + bestSquare->bottomLeft.y) / 2) + (randomOffset);
					foundPoint = true;
				}
			}
		}

		if (!foundPoint) {

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

			//if spawn point not found after 10 attempts
			if (maxAcceptableLoops-- == 0) {

				//reset number of attempts
				maxAcceptableLoops = 10;
				//increase the number of points allowed in a square
				minAcceptablePoints++;
				//randomize the list again for good measure
				RandomizeMapSquareList(mapSquareList);
			}
		}

	//function to check the spawn point generated
	} while (!IsSpawnPointValid(spawnPoint));

	return PxVec3(spawnPoint.x, spawnHeight, spawnPoint.y);
}

/*
* PUBLIC FUNCTIONS
*/

void SharedDataSystem::MAKE_BOX_DEBUG(PxReal x, PxReal z, PxReal y) {

	if (DEBUG_BOXES) {

		//define a box
		physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(0.25f, 0.25f, 0.25f), *gMaterial);
		PxRigidStatic* boxBody = gPhysics->createRigidStatic(PxTransform(x, y, z));
		boxBody->setName("DEBUG BOX");
		boxBody->attachShape(*shape);
		boxBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);
		gScene->addActor(*boxBody);
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

	//has some code re-use for easy function usage

	std::vector<PxVec2> locations;
	PxVec3 respawnPoint;

	//uses physics type to determine min spacing
	switch (physType) {
	case PhysicsType::CAR:

		for (int i = 0; i < carInfoList.size(); i++) {

			//dont care about location of dead cars
			if (carInfoList[i].isAlive) {
				locations.emplace_back(carInfoList[i].entity->collisionBox->getGlobalPose().p.x, carInfoList[i].entity->collisionBox->getGlobalPose().p.z);
			}
		}

		//place the points in their respecitve squares
		PopulateMapSquareList(locations, carMapSquareList);

		//randomize the list to prevent spawning in the same place always
		RandomizeMapSquareList(carMapSquareList);

		return GenerateValidSpawnPoint(carMapSquareList, locations, CAR_MIN_SPAWN_DISTANCE, CAR_SPAWN_HEIGHT);

		break;
	case PhysicsType::POWERUP:

		for (int i = 0; i < allPowerupList.size(); i++) {
			locations.emplace_back(PxVec2(allPowerupList[i].entity->collisionBox->getGlobalPose().p.x, allPowerupList[i].entity->collisionBox->getGlobalPose().p.z));
		}

		//place the points in their respecitve squares
		PopulateMapSquareList(locations, powerupMapSquareList);

		//randomize the list to prevent spawning in the same place always
		RandomizeMapSquareList(powerupMapSquareList);

		return GenerateValidSpawnPoint(powerupMapSquareList, locations, POWERUP_MIN_SPAWN_DISTANCE, POWERUP_SPAWN_HEIGHT);

		break;
	default:
		printf("this is a physics type that cannot be respawned");
		break;
	}

	//unreachable code
	printf("Failed to Determine Spawn Location.");
	exit(69);
}

void SharedDataSystem::AddObstacleToObstacleList(PxRigidStatic* obstacle) {

	//getting half the size of the object
	PxVec3 halfSize = obstacle->getWorldBounds().getDimensions() / 2;
	
	//making the box that cant spawn in bigger to avoid clipping issues
	halfSize.x += STATIC_SPAWN_OFFSET;
	halfSize.z += STATIC_SPAWN_OFFSET;

	//getting the center of the object
	PxVec3 center = obstacle->getGlobalPose().p;

	//creating the mapsquare for the obstacle
	MapSquare obstacleMapSquare;
	obstacleMapSquare.bottomLeft = PxVec2(center.x - halfSize.x, center.z - halfSize.z);
	obstacleMapSquare.topRight = PxVec2(center.x + halfSize.x, center.z + halfSize.z);

	//adding it to the list
	obstacleMapSquareList.emplace_back(obstacleMapSquare);
}

bool SharedDataSystem::Parry(PxRigidDynamic* carThatParried) {

	CarInfo* carInfo = GetCarInfoStructFromEntity(GetEntityFromRigidDynamic(carThatParried));

	//can sucessfully parry
	if (carInfo->parryCooldownTimeLeft < 0 && carInfo->isAlive) {

		carInfo->parryActiveTimeLeft = PARRY_ACTIVE_DURATION;
		carInfo->parryCooldownTimeLeft = PARRY_COOLDOWN_TIME_LEFT;

		return true;
	}
	
	return false;
}

std::shared_ptr<Entity> SharedDataSystem::GetCarThatShotProjectile(PxRigidDynamic* projectile) {

	//not sure if there is a better way to do this
	//go through every list of shot projectiles and search for the passed one
	//return the car
	for (auto iterator = carProjectileRigidDynamicDict.begin(); iterator != carProjectileRigidDynamicDict.end(); ++iterator) {

		for (PxRigidDynamic* listProjectile : iterator->second) {
			if (listProjectile == projectile) {
				if (DEBUG_PRINTS) printf("GetCarThatShotProjectile hard debug\n");
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

void SharedDataSystem::AddToCollatCache(std::shared_ptr<Entity> entityToAdd) {

	//adding the projectile to the collat cache
	bool isInCache = false;
	for (int i = 0; i < collatCache.size(); i++) {
		if (collatCache[i]->name == entityToAdd->name) {
			isInCache = true;
			break;
		}
	}

	//only add it to the cache if it isnt already in there
	if (!isInCache) {
		collatCache.emplace_back(entityToAdd);
	}
}

void SharedDataSystem::CarProjectileCollisionLogic(PxActor* car, PxActor* projectile) {

	if (DEBUG_PRINTS) printf("CarProjectileCollisionLogic before\n");

	std::shared_ptr<Entity> shotCarEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);

	if (DEBUG_PRINTS) printf("CarProjectileCollisionLogic after\n");

	//get the shooting cars rigid dynamic
	PxRigidDynamic* shootingCarRigidDynamic = GetCarThatShotProjectile((PxRigidDynamic*)projectile)->collisionBox;

	//car info struct of the shot car
	CarInfo* shotCarInfo = GetCarInfoStructFromEntity(shotCarEntity);

	//if the shot car has parried
	if (shotCarInfo->parryActiveTimeLeft > 0) {

		//playing a parry sound
		SoundsToPlay.push_back(std::make_pair(std::string("Parry"), shotCarEntity->collisionBox->getGlobalPose().p));

		//change ownership of the projectile
		for (int i = 0; i < carProjectileRigidDynamicDict[shootingCarRigidDynamic].size(); i++) {
			if (carProjectileRigidDynamicDict[shootingCarRigidDynamic][i] == (PxRigidDynamic*)projectile) {
				carProjectileRigidDynamicDict[(PxRigidDynamic*)car].emplace_back((PxRigidDynamic*)projectile);
				carProjectileRigidDynamicDict[shootingCarRigidDynamic].erase(carProjectileRigidDynamicDict[shootingCarRigidDynamic].begin() + i);
			}
		}

		//getting the new forward direction of the projectile
		PxVec3 projectileBackwardVector = projectileEntity->collisionBox->getGlobalPose().q.getBasisVector2() * -1;

		//getting the actual radius of the projectile (to not worry about powerups)
		float projectileRadius = projectile->getWorldBounds().getExtents().x;

		//send the projectile back the way it came
			//doing the offset based on the same math as the shooting math
		projectileEntity->collisionBox->setGlobalPose(
			PxTransform(
				PxVec3(
					projectileEntity->collisionBox->getGlobalPose().p.x + projectileBackwardVector.x * projectileRadius * 6.5,
					projectileEntity->collisionBox->getGlobalPose().p.y,
					projectileEntity->collisionBox->getGlobalPose().p.z + projectileBackwardVector.x * projectileRadius * 6.5),
				projectileEntity->collisionBox->getGlobalPose().q));

		//stoled from car shoot ahahah
		projectileEntity->collisionBox->setLinearVelocity(SHOOT_FORCE * PxVec3(projectileBackwardVector.x, 0, projectileBackwardVector.z));

		//update cooldowns
		shotCarInfo->parryActiveTimeLeft = 0;
		shotCarInfo->parryCooldownTimeLeft = PARRY_COOLDOWN_TIME_LEFT;

	}
	//armour tanks the bullet
	else if (shotCarInfo->hasArmour) {

		shotCarInfo->hasArmour = false;

		AddToCollatCache(projectileEntity);

		SoundsToPlay.push_back(std::make_pair(std::string("Armour"), getSoundRotMat() * shotCarEntity->collisionBox->getGlobalPose().p));
	}
	else {

		//increase score of car that shot
		CarInfo* shootingCarInfo = GetCarInfoStructFromEntity(GetCarThatShotProjectile((PxRigidDynamic*)projectile));
		shootingCarInfo->score++;

		AddToCollatCache(projectileEntity);
		
		//setting the data of the car that got hit to let it respawn
		CarInfo* hitCar = GetCarInfoStructFromEntity(shotCarEntity);
		hitCar->respawnTimeLeft = CAR_RESPAWN_LENGTH;
		hitCar->isAlive = false;
		//moving into the sky and disabling gravity to "delete it"
		hitCar->entity->collisionBox->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);
		PxReal yShift = hitCar->entity->collisionBox->getGlobalPose().p.y + 150;
		PxVec3 carShift(hitCar->entity->collisionBox->getGlobalPose().p.x, yShift, hitCar->entity->collisionBox->getGlobalPose().p.z);
		hitCar->entity->collisionBox->setGlobalPose(PxTransform(carShift));

		//diff sounds depending on who was killed
		if (shotCarEntity->name == carInfoList[0].entity->name) {
			//make a sound
			SoundsToPlay.push_back(std::make_pair(std::string("Heaven"), getSoundRotMat() * shotCarEntity->collisionBox->getGlobalPose().p));
		}
		else {
			//make a sound
			SoundsToPlay.push_back(std::make_pair(std::string("Bwud"), getSoundRotMat() * shotCarEntity->collisionBox->getGlobalPose().p));
		}
	
	}

}

void SharedDataSystem::CarPowerupCollisionLogic(PxActor* car, PxActor* powerup) {

	if (DEBUG_PRINTS) printf("CarPowerupCollisionLogic before\n");

	//converting the actors to entities
	std::shared_ptr<Entity> carEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	std::shared_ptr<Entity> powerupEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)powerup);

	if (DEBUG_PRINTS) printf("CarPowerupCollisionLogic after\n");

	//gives the car the powerups effect
	switch (GetPowerupInfoStructFromEntity(powerupEntity)->powerupType) {
	case PowerupType::AMMO:

		GetCarInfoStructFromEntity(carEntity)->ammoCount += NUMBER_AMMO_GIVEN_PER_POWERUP;
		break;
	case PowerupType::ARMOUR:

		GetCarInfoStructFromEntity(carEntity)->hasArmour = true;
		break;
	case PowerupType::PROJECTILESIZE:

		GetCarInfoStructFromEntity(carEntity)->projectileSizeActiveTimeLeft = PROJECTILE_SIZE_POWERUP_DURATION;
		break;
	case PowerupType::PROJECTILESPEED:

		GetCarInfoStructFromEntity(carEntity)->projectileSpeedActiveTimeLeft = PROJECTILE_SPEED_POWERUP_DURATION;
		break;
	case PowerupType::CARSPEED:

		break;
	default:
		printf("unknown powerup type\n");
		break;
	}

	AddToCollatCache(powerupEntity);

}

void SharedDataSystem::ProjectileStaticCollisionLogic(PxActor* projectile) {

	if (DEBUG_PRINTS) printf("ProjectileStaticCollisionLogic before\n");

	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);
	PxRigidDynamic* carThatShotProjectile = GetCarThatShotProjectile((PxRigidDynamic*)projectile)->collisionBox;

	if (DEBUG_PRINTS) printf("ProjectileStaticCollisionLogic after\n");

	AddToCollatCache(projectileEntity);

}

void SharedDataSystem::CleanCollatCache() {

	std::shared_ptr<Entity> shootingCar;

	while (collatCache.size() > 0) {

		switch (collatCache.front()->physType) {
		case PhysicsType::PROJECTILE:

			shootingCar = GetCarThatShotProjectile(collatCache.front()->collisionBox);

			//remove projectile from car projectile dict
			for (int i = 0; i < carProjectileRigidDynamicDict[shootingCar->collisionBox].size(); i++) {
				if (carProjectileRigidDynamicDict[shootingCar->collisionBox][i] == collatCache.front()->collisionBox) {
					carProjectileRigidDynamicDict[shootingCar->collisionBox].erase(carProjectileRigidDynamicDict[shootingCar->collisionBox].begin() + i);
				}
			}

			break;
		case PhysicsType::POWERUP:

			//all powerup list
			for (int i = 0; i < allPowerupList.size(); i++) {
				if (allPowerupList[i].entity->name == collatCache.front()->name) {
					allPowerupList.erase(allPowerupList.begin() + i);
				}
			}

			break;
		default:
			break;
		}

		//delete the object
		gScene->removeActor(*collatCache.front()->collisionBox);
		collatCache.front()->collisionBox->release();

		//remove from entity list
		for (int i = 0; i < entityList.size(); i++) {
			if (entityList[i].name == collatCache.front()->name) {
				entityList.erase(entityList.begin() + i);
			}
		}

		//remove it from the cache
		collatCache.erase(collatCache.begin());

	}
}

void SharedDataSystem::ResolveCollisions() {

	//if a collision has occured
	if (gContactReportCallback->contactDetected) {

		//goes through all contact pairs
		for (int i = 0; i < SharedDataSystem::contactPairs.size(); i++) {

			//code readability variables
			PxActor* actor1 = SharedDataSystem::contactPairs[i].actors[0];
			PxActor* actor2 = SharedDataSystem::contactPairs[i].actors[1];

			if (DEBUG_PRINTS) printf("ResolveCollisions before\n");

			//get the two entities that collided
			std::shared_ptr<Entity> entity1 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor1);
			std::shared_ptr<Entity> entity2 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor2);

			if (DEBUG_PRINTS) printf("ResolveCollisions after\n");

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

	}

	//resolved the collisions
	SharedDataSystem::contactPairs.clear();
	gContactReportCallback->contactDetected = false;
}

void SharedDataSystem::InitMapSquares(std::vector<MapSquare>& listToPopulate, PxReal minDistance) {

	int xMapSquares = MAPLENGTHX / minDistance;
	int zMapSquares = MAPLENGTHZ / minDistance;

	//divide the map into squares using the minDistance
	for (int i = 0; i < xMapSquares; i++) {
		for (int j = 0; j < zMapSquares; j++) {
			MapSquare square;
			square.id = i * zMapSquares + j;
			square.bottomLeft = PxVec2(BOTTOM_LEFT_MAP_COORD.x + i * minDistance, BOTTOM_LEFT_MAP_COORD.y + j * minDistance);
			square.topRight = PxVec2(BOTTOM_LEFT_MAP_COORD.x + (i + 1) * minDistance, BOTTOM_LEFT_MAP_COORD.y + (j + 1) * minDistance);
			listToPopulate.emplace_back(square);

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
			square.id = listToPopulate.size() + 1;
			square.bottomLeft = PxVec2(TOP_RIGHT_MAP_COORD.x - remainingX, i);
			square.topRight = PxVec2(TOP_RIGHT_MAP_COORD.x, i + minDistance);
			listToPopulate.emplace_back(square);
		}
	}

	//add semi nice squares in the x direction
	if (remainingZ > 0) {

		//go from z = bottom left map z in increments of minDistance until hit either top right or just less than
		//constant x size

		//loops "right" in the x-direction
		for (int i = BOTTOM_LEFT_MAP_COORD.x; i < TOP_RIGHT_MAP_COORD.x - minDistance; i += minDistance) {

			MapSquare square;
			square.id = listToPopulate.size() + 1;
			square.bottomLeft = PxVec2(i, TOP_RIGHT_MAP_COORD.y - remainingZ);
			square.topRight = PxVec2(i + minDistance, TOP_RIGHT_MAP_COORD.y);
			listToPopulate.emplace_back(square);
		}

	}

	//add the corner fucked square
	if (remainingX > 0 && remainingZ > 0) {
		MapSquare square;
		square.id = listToPopulate.size() + 1;
		square.bottomLeft = PxVec2(TOP_RIGHT_MAP_COORD.x - remainingX, TOP_RIGHT_MAP_COORD.y - remainingZ);
		square.topRight = TOP_RIGHT_MAP_COORD;
		listToPopulate.emplace_back(square);
	}

	//checks all boxes against all obstacles
	//checks all the elements using fancy iterator logic
	std::vector<MapSquare>::iterator iterator = listToPopulate.begin();
	do {

		iterator++;

		//go through all obstacles
		for (int i = 0; i < obstacleMapSquareList.size(); i++) {

			//if the bottom left and top right are in any obstacle
				//may lead to some deadzones around obstacles
			if (IsPointInSquare((iterator - 1)->bottomLeft, obstacleMapSquareList[i])) {

				if (IsPointInSquare((iterator - 1)->topRight, obstacleMapSquareList[i])) {

					//remove the map square
					iterator = listToPopulate.erase(iterator - 1);
					break;
				}
			}
		}
	} while (iterator != listToPopulate.end());

	//makes all debug boxes for the remaining map squares
	for (int i = 0; i < listToPopulate.size(); i++) {

		MAKE_BOX_DEBUG(listToPopulate[i].bottomLeft.x, listToPopulate[i].bottomLeft.y);
		MAKE_BOX_DEBUG(listToPopulate[i].topRight.x, listToPopulate[i].topRight.y);

		MAKE_BOX_DEBUG(listToPopulate[i].bottomLeft.x, listToPopulate[i].topRight.y);
		MAKE_BOX_DEBUG(listToPopulate[i].topRight.x, listToPopulate[i].bottomLeft.y);
	}

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

	//add all the static objects to the entity list
	for (int i = 0; i < STATIC_OBJECT_LIST.size(); i++) {

		entityList.emplace_back(STATIC_OBJECT_LIST[i]);
	}
}

void SharedDataSystem::InitSharedDataSystem() {

	//generate the map squares for both cars and powerups
	InitMapSquares(this->carMapSquareList, CAR_MIN_SPAWN_DISTANCE);
	InitMapSquares(this->powerupMapSquareList, POWERUP_MIN_SPAWN_DISTANCE);

	//make all obstacle debug boxes
	for (int i = 0; i < obstacleMapSquareList.size(); i++) {

		MAKE_BOX_DEBUG(obstacleMapSquareList[i].bottomLeft.x, obstacleMapSquareList[i].bottomLeft.y, 10);
		MAKE_BOX_DEBUG(obstacleMapSquareList[i].topRight.x, obstacleMapSquareList[i].topRight.y, 10);

		MAKE_BOX_DEBUG(obstacleMapSquareList[i].bottomLeft.x, obstacleMapSquareList[i].topRight.y, 10);
		MAKE_BOX_DEBUG(obstacleMapSquareList[i].topRight.x, obstacleMapSquareList[i].bottomLeft.y, 10);
	}
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

PxMat33 SharedDataSystem::getRotMatPx(float angle) {
	return PxMat33({ cos(angle), 0, sin(angle) }, { 0, 1, 0 }, { -sin(angle), 0, -cos(angle) });
}

PxMat33 SharedDataSystem::getSoundRotMat() {
	return PxMat33({ cos((float)M_PI - cameraAngle), 0, sin((float)M_PI - cameraAngle) }, { 0, 1, 0 }, { -sin((float)M_PI - cameraAngle), 0, -cos((float)M_PI - cameraAngle) });
}