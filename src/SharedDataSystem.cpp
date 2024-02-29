#include "SharedDataSystem.h"

EngineDriveVehicle* SharedDataSystem::GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic) {
	
	for (int i = 0; i < carRigidDynamicList.size(); i++) {

		if (carRigidDynamicList[i] == carRigidDynamic) {
			return gVehicleList[i];
		}
	}

	//unreachable code
	exit(69);
}

Entity* SharedDataSystem::GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic) {

	for (int i = 0; i < entityList.size(); i++) {

		if (entityList.at(i).collisionBox == rigidDynamic) {
			return &entityList.at(i);
		}
	}
	//unreachable code
	exit(69);
}

void SharedDataSystem::CarProjectileCollisionLogic(PxActor* car, PxActor* projectile) {

	Entity* carEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)car);
	Entity* projectileEntity = GetEntityFromRigidDynamic((PxRigidDynamic*)projectile);

	//increase score of car that shot (not figured out yet)
		//data struct that associates a score with either car entity or gvehicle...

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

	//respawn shit (not figured out yet)

}

void SharedDataSystem::CarPowerupCollisionLogic(PxActor* car, PxActor* powerup) {
	printf("car hit powerup\n");
}

void SharedDataSystem::ProjectileStaticCollisionLogic(PxActor* projectile) {
	printf("projectile hit static\n");
}

void SharedDataSystem::ResolveCollisions() {

	//if a collision has occured
	if (gContactReportCallback->contactDetected) {

		//code readability variables
		PxActor* actor1 = gContactReportCallback->contactPair.actors[0];
		PxActor* actor2 = gContactReportCallback->contactPair.actors[1];

		//get the two entities that collided
		Entity* entity1 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor1);
		Entity* entity2 = GetEntityFromRigidDynamic((PxRigidDynamic*)actor2);

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