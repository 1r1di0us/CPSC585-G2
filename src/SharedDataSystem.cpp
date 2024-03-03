#include "SharedDataSystem.h"
#include <queue>

CarInfo* SharedDataSystem::GetCarInfoStructFromEntity(std::shared_ptr<Entity> entity) {
	
	for (int i = 0; i < carInfoList.size(); i++) {
		if (carInfoList[i].entity->name == entity->name) {
			return &carInfoList[i];
		}
	}

	//unreachable code
	exit(69);
}

EngineDriveVehicle* SharedDataSystem::GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic) {
	
	for (int i = 0; i < carRigidDynamicList.size(); i++) {

		if (carRigidDynamicList[i] == carRigidDynamic) {
			return gVehicleList[i];
		}
	}

	//unreachable code
	exit(69);
}

PxRigidDynamic* SharedDataSystem::GetRigidDynamicFromVehicle(EngineDriveVehicle* gVehicle) {

	for (int i = 0; i < gVehicleList.size(); i++) {

		if (gVehicleList[i] == gVehicle) {
			return carRigidDynamicList[i];
		}
	}

	//unreachable code
	exit(69);
}

std::shared_ptr<Entity> SharedDataSystem::GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic) {

	for (int i = 0; i < entityList.size(); i++) {

		if (entityList.at(i).collisionBox == rigidDynamic) {
			return std::make_shared<Entity>(entityList[i]);
		}
	}

	//unreachable code
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

PxVec2 SharedDataSystem::ConvertVec3ToVec2(PxVec3 vectorToConvert) {
	return PxVec2(vectorToConvert.x, vectorToConvert.z);
}

bool SharedDataSystem::IsPointInSquare(PxVec2 point, MapSquare& square) {

	//check if the point is between the two other points. on the line counts
	if (point.x < square.bottomLeft.x || point.x > square.topRight.x ||
		point.y < square.bottomLeft.y || point.y > square.topRight.y) {
		return false;
	}

	return true;
}

// Function to calculate the distance between two PhysX vectors
float distanceBetween(const PxVec2& point1, const PxVec2& point2) {
	return (point1 - point2).magnitude();
}

// Comparator for priority queue based on distance
struct CompareDistance {
	bool operator()(const std::pair<float, PxVec2>& p1, const std::pair<float, PxVec2>& p2) {
		return p1.first < p2.first;
	}
};

std::vector<PxVec2> SharedDataSystem::GetXNearestPoints(std::vector<PxVec2> pointList, int numPointsToGet, std::vector<PxVec2> pointsOfSameType) {
	
	std::priority_queue<std::pair<float, PxVec2>, std::vector<std::pair<float, PxVec2>>, CompareDistance> minHeap;

	for (const auto& point : pointsOfSameType) {
		float distance = 0.0f;
		for (const auto& p : pointList) {
			distance += distanceBetween(point, p);
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

PxVec2 SharedDataSystem::FindCenterOfFourPoints(std::vector<PxVec2> pointsList) {
	float centerX = (pointsList[0].x + pointsList[1].x + pointsList[2].x + pointsList[3].x) / 4.0f;
	float centerY = (pointsList[0].y + pointsList[1].y + pointsList[2].y + pointsList[3].y) / 4.0f;

	return PxVec2(centerX, centerY);
}

void SharedDataSystem::PopulateMapSquareList(std::vector<PxVec2> pointsOfSameType, std::vector<MapSquare>& mapSquareList) {

	//go through the map square list and populate each one fully before moving on to the next one
	for (int i = 0; i < mapSquareList.size(); i++) {
		while (pointsOfSameType.size() > 0) {
			if (IsPointInSquare(pointsOfSameType.at(0), mapSquareList[i])) {
				mapSquareList[i].numPoints++;
				mapSquareList[i].pointsInIt.emplace_back(pointsOfSameType.at(0));
				pointsOfSameType.erase(pointsOfSameType.begin());
				break;
			}
		}
	}
}

PxVec3 SharedDataSystem::GenerateSpawnPoint(std::vector<PxVec2> pointsOfSameType, PxReal minDistance, PxReal spawnHeight) {

	PxVec2 spawnPoint;
	std::vector<MapSquare> mapSquareList;

	int xMapSquares = MAPLENGTHX / minDistance;
	int zMapSquares = MAPLENGTHZ / minDistance;

	//divide the map into squares using the minDistance
	for (int i = 0; i < xMapSquares * zMapSquares; i++) {
		MapSquare square;
		square.id = i;
		square.bottomLeft = PxVec2(BOTTOMLEFTMAPCOORD.x + i * minDistance, BOTTOMLEFTMAPCOORD.y + i * minDistance);
		square.topRight = PxVec2(TOPRIGHTMAPCOORD.x + i * minDistance, TOPRIGHTMAPCOORD.y + i * minDistance);
		mapSquareList.emplace_back(square);
	}

	//place the points in their respecitve squares
	PopulateMapSquareList(pointsOfSameType, mapSquareList);

	//find the square with the least amount of points in it
	//if the square has no points in it, find the center and return that
	MapSquare* bestSquare = &mapSquareList[0];

	for (int i = 0; i < mapSquareList.size(); i++) {
		if (mapSquareList[i].numPoints < bestSquare->numPoints) {
			bestSquare = &mapSquareList[i];
			if (bestSquare->numPoints == 0) {
				spawnPoint.x = (bestSquare->topRight.x - bestSquare->bottomLeft.x) / 2;
				spawnPoint.y = (bestSquare->topRight.y - bestSquare->bottomLeft.y) / 2;
				return PxVec3(spawnPoint.x, spawnHeight, spawnPoint.y);
			}
		}
	}

	switch (bestSquare->numPoints) {
	case 1:
		//need to find 3 nearest points
		spawnPoint = FindCenterOfFourPoints(GetXNearestPoints(bestSquare->pointsInIt, 3, pointsOfSameType));
		
		break;
	case 2:
		//need to find 2 nearest points
		spawnPoint = FindCenterOfFourPoints(GetXNearestPoints(bestSquare->pointsInIt, 2, pointsOfSameType));
		break;
	case 3:
		//need to find nearest point
		spawnPoint = FindCenterOfFourPoints(GetXNearestPoints(bestSquare->pointsInIt, 1, pointsOfSameType));
		break;
	case 4:
		//make square, return middle
		spawnPoint = FindCenterOfFourPoints(bestSquare->pointsInIt);
		break;
	default:
		//make a random point in the square and return that
		spawnPoint.x = std::rand() / static_cast<double>(RAND_MAX) * minDistance;
		spawnPoint.y = std::rand() / static_cast<double>(RAND_MAX) * minDistance;
		break;
	}

	return PxVec3(spawnPoint.x, spawnHeight, spawnPoint.y);
}

PxVec3 SharedDataSystem::DetermineSpawnLocation(PhysicsType physType) {

	std::vector<PxVec2> locations;

	//the spacing from other entities of the same physics type is important
	switch (physType) {
	case PhysicsType::CAR:

		for (int i = 0; i < carRigidDynamicList.size(); i++) {
			locations.emplace_back(PxVec2(carRigidDynamicList[i]->getGlobalPose().p.x, carRigidDynamicList[i]->getGlobalPose().p.z));
		}

		return GenerateSpawnPoint(locations, CARMINSPAWNDISTANCE, CARSPAWNHEIGHT);

		break;
	case PhysicsType::POWERUP:

		return PxVec3(0, 0, 0);
		break;
	default:
		printf("this is a physics type that cannot be spawned");
		break;
	}

	//unreachable code
	exit(69);
}

std::shared_ptr<Entity> SharedDataSystem::GetCarThatShotProjectile(PxRigidDynamic* projectile) {

	//not sure if there is a better way to do this
	//go through every list of shot projectiles and search for the passed one
	//return the car
	for (auto iterator = carProjectileRigidDynamicDict.begin(); iterator != carProjectileRigidDynamicDict.end(); ++iterator) {
		
		for (PxRigidDynamic* listProjectile : iterator->second) {
			if (listProjectile == projectile) {
				return GetEntityFromRigidDynamic(iterator->first);
			}
		}
	}

	//unreachable code
	exit(69);

}

void SharedDataSystem::CarProjectileCollisionLogic(PxActor* car, PxActor* projectile) {

	std::shared_ptr<Entity> carEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);

	//increase score of car that shot
	CarInfo* shootingCarInfo = GetCarInfoStructFromEntity(GetCarThatShotProjectile((PxRigidDynamic*)projectile));
	shootingCarInfo->score++;

	printf("score of %s: %d\n", shootingCarInfo->entity->name.c_str(), shootingCarInfo->score);

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

	//setting the data of the car that got hit to let it respawn
	CarInfo* hitCar = GetCarInfoStructFromEntity(carEntity);
	hitCar->respawnTimeLeft = RESPAWNLENGTH;
	hitCar->isAlive = false;
	//moving into the sky and disabling gravity to "delete it"
	hitCar->entity->collisionBox->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);
	PxReal yShift = hitCar->entity->collisionBox->getGlobalPose().p.y + 100;
	PxVec3 carShift(hitCar->entity->collisionBox->getGlobalPose().p.x, yShift, hitCar->entity->collisionBox->getGlobalPose().p.z);
	hitCar->entity->collisionBox->setGlobalPose(PxTransform(carShift));

}

void SharedDataSystem::CarPowerupCollisionLogic(PxActor* car, PxActor* powerup) {
	printf("car hit powerup\n");
}

void SharedDataSystem::ProjectileStaticCollisionLogic(PxActor* projectile) {

	std::shared_ptr<Entity> projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);

	//entity list
	for (int i = 0; i < entityList.size(); i++) {
		if (entityList[i].name == projectileEntity->name) {
			entityList.erase(entityList.begin() + i);
		}
	}

	//car projectile dict
	for (auto& entry : carProjectileRigidDynamicDict) {
		for (int i = 0; i < entry.second.size(); i++) {
			if (carProjectileRigidDynamicDict[(PxRigidDynamic*)entry.first][i] == (PxRigidDynamic*)projectile) {
				carProjectileRigidDynamicDict[(PxRigidDynamic*)entry.first].erase(carProjectileRigidDynamicDict[(PxRigidDynamic*)entry.first].begin() + i);
			}
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

		//get the two entities that collided
		std::shared_ptr<Entity> entity1 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor1);
		std::shared_ptr<Entity> entity2 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor2);

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
				CarProjectileCollisionLogic(actor2, actor1);
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