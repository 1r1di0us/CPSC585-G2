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

	//function to spawn all powerups (creating them initially)
	void SpawnAllPowerups();

	//not actually gonna delete them (just move them into the sky)
	//works same as car respawn
	void RespawnAllPowerups();
};