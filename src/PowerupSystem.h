#pragma once

#include "SharedDataSystem.h";

enum PowerupType {
	AMMO,
	PROJECTILESPEED,
	PROJECTILESIZE,
	CARSPEED
};

class PowerupSystem {

private:
	//a reference to the instance of the shared data system in carsystem
	SharedDataSystem* dataSys;

public:

	//constructor
	PowerupSystem(SharedDataSystem* dataSys);

	//function to spawn a powerup
	void SpawnPowerup(PxVec3 spawnPosition, PowerupType powerupType);
};