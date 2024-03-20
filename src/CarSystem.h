#pragma once

#include <vector>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"
#include "Entity.h"
#include <map>
#include "SharedDataSystem.h"

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

//Commands are issued to the vehicle in a pre-choreographed sequence.
//not used atm
struct Command
{
	PxF32 brake;
	PxF32 throttle;
	PxF32 steer;
	PxU32 gear;
	PxF32 duration;
};

class CarSystem {

private:

	//a reference to the instance of the shared data system in carsystem
	SharedDataSystem* dataSys;

public:

	//The path to the vehicle json files to be loaded.
	const char* gVehicleDataPath = "assets/vehicledata";

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	//automatic transmission
	const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//constructor
	CarSystem(SharedDataSystem* dataSys);

	//spawns a new car at given position and rotation
	void SpawnNewCar(PxVec2 spawnPosition, PxQuat spawnRotation);

	//respawn all dead cars
	void RespawnAllCars();

	/*
	* PROJECTILES
	*/

	//constants
	const PxReal projectileMass = 10.0f;

	//shoots a projectile from the given car
	bool Shoot(PxRigidDynamic* shootingCar);

	//updates all the car cooldowns
	void UpdateAllCarCooldowns();

};