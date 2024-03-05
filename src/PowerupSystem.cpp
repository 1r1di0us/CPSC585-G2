#include "PowerupSystem.h"

PowerupSystem::PowerupSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;
}

std::string PowerupSystem::PowerupTypeToString(PowerupType powerupType) {
	switch (powerupType) {
	case PowerupType::AMMO:
		return "AMMO";
	case PowerupType::PROJECTILESPEED:
		return "PROJECTILESPEED";
	case PowerupType::PROJECTILESIZE:
		return "PROJECTILESIZE";
	case PowerupType::CARSPEED:
		return "CARSPEED";
	default:
		printf("UNKOWN POWERUP TYPE");
		exit(69);
	}
}

void PowerupSystem::SpawnPowerup(PxVec3 spawnPosition, PowerupType powerupType) {
	
	//define a powerup
	PxShape* shape = dataSys->gPhysics->createShape(PxBoxGeometry(PxVec3(0.5f, 0.5f, 0.5f)), *dataSys->gMaterial);
	
	//makes the shape a trigger volume (cant be both a simulation shape and trigger volume at same time)
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	//creating collision flags for each powerup
	physx::PxFilterData powerupFilter(COLLISION_FLAG_POWERUP, COLLISION_FLAG_POWERUP_AGAINST, 0, 0);
	shape->setSimulationFilterData(powerupFilter);

	//creates the rigid dynamic body for the powerup
	PxRigidDynamic* powerupBody = dataSys->gPhysics->createRigidDynamic(PxTransform(spawnPosition));
	
	//disables gravity for the powerup
	powerupBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	powerupBody->attachShape(*shape);

	dataSys->gScene->addActor(*powerupBody);

	//creating the powerup entity to add to the entity list
	Entity powerup;
	powerup.name = PowerupTypeToString(powerupType) + "powerup" + std::to_string(dataSys->allPowerupList.size());
	powerup.CreateTransformFromPhysX(PxTransform(spawnPosition));
	powerup.physType = PhysicsType::POWERUP;
	powerup.collisionBox = powerupBody;

	dataSys->entityList.emplace_back(powerup);

	//making the powerup info to add to the list
	PowerupInfo powerupInfo;
	powerupInfo.entity = std::make_shared<Entity>(powerup);
	powerupInfo.powerupType = powerupType;

	dataSys->allPowerupList.emplace_back(powerupInfo);
}

void PowerupSystem::SpawnAllPowerups() {

	//for loops calling spawn powerup based on number of each one wanted

}

void PowerupSystem::RespawnAllPowerups() {

	//TODO: rework

	//go through all dead powerups
	//for (PowerupInfo* powerupInfo : dataSys->GetListOfDeadPowerups()) {

	//	//if the car is ready to be respawned
	//	if (powerupInfo->respawnTimeLeft <= 0) {

	//		//get the spawn location
	//		PxVec3 spawnVec = dataSys->DetermineRespawnLocation(PhysicsType::POWERUP);

	//		//"spawn" the powerup
	//		powerupInfo->needsRespawn = false;
	//		powerupInfo->respawnTimeLeft = 0;
	//	}
	//	else {

	//		//subtract the physics system update rate from the respawn timer
	//			//real time instead maybe?
	//		powerupInfo->respawnTimeLeft -= dataSys->TIMESTEP;
	//	}
	//}
}

