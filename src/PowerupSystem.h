#pragma once

#include <string>
#include "SharedDataSystem.h"

class PowerupSystem {

private:

	//a reference to the instance of the shared data system in carsystem
	SharedDataSystem* dataSys;

public:

	//constructor
	PowerupSystem(SharedDataSystem* dataSys);

	//converts the powerup type to string
	std::string PowerupTypeToString(PowerupType powerupType);

	//function to spawn a powerup
	void SpawnPowerup(PxVec3 spawnPosition, PowerupType powerupType);

	//creates new powerups depending on time variables
	void RespawnAllPowerups();
};