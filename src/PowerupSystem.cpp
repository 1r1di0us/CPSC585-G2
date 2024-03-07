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
	powerup.name = PowerupTypeToString(powerupType) + "powerup" + std::to_string(dataSys->spawnedPowerupCounter++);
	powerup.CreateTransformFromPhysX(PxTransform(spawnPosition));
	powerup.physType = PhysicsType::POWERUP;
	powerup.collisionBox = powerupBody;

	powerupBody->setName(powerup.name.c_str());

	dataSys->entityList.emplace_back(powerup);

	//making the powerup info to add to the list
	PowerupInfo powerupInfo;
	powerupInfo.entity = std::make_shared<Entity>(powerup);
	powerupInfo.powerupType = powerupType;

	dataSys->allPowerupList.emplace_back(powerupInfo);
}

void PowerupSystem::RespawnAllPowerups() {

	//decrement both respawn timers
	dataSys->timeUntilRandomPowerup -= dataSys->TIMESTEP;
	dataSys->timeUntilAmmoPowerup -= dataSys->TIMESTEP;
	
	//spawning a new random powerup
	if (dataSys->timeUntilRandomPowerup <= 0) {

		PxVec3 spawnVec = dataSys->DetermineRespawnLocation(PhysicsType::POWERUP);

		// Seed the random number generator
			//might be worth having the generator based on the real time used in game
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		// Get the total number of enum values
		int numPowerupTypes = static_cast<int>(PowerupType::NUM_POWERUP_TYPES);

		// Generate a random index within the range of enum values
		int randomPowerupType = std::rand() % numPowerupTypes;

		SpawnPowerup(spawnVec, static_cast<PowerupType>(randomPowerupType));

		//resetting the timer
		dataSys->timeUntilRandomPowerup = dataSys->RANDOM_POWERUP_SPAWN_RATE;
		
	}

	//spawning a new ammo powerup
	if (dataSys->timeUntilAmmoPowerup <= 0) {

		PxVec3 spawnVec = dataSys->DetermineRespawnLocation(PhysicsType::POWERUP);

		SpawnPowerup(spawnVec, PowerupType::AMMO);

		//resetting the timer
		dataSys->timeUntilAmmoPowerup = dataSys->AMMO_POWERUP_SPAWN_RATE;
	}
}

