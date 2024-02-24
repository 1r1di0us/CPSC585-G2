#pragma once

#include <vector>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"
#include "Entity.h"
#include <map>

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

public:

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxMaterial* gMaterial;

	std::vector<Entity>* entityList;

	//The path to the vehicle json files to be loaded.
	const char* gVehicleDataPath = "assets/vehicledata";

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	//automatic transmission
	const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//need to have list of rigid dynamics corresponding to gvehicles vehicles to move the correct vehicle given rigid dynamic
	std::vector<PxRigidDynamic*> carRigidDynamicList;
	std::vector<EngineDriveVehicle*> gVehicleList;

	//constructor
	CarSystem(PxPhysics* gPhysics, PxScene* gScene, PxMaterial* gMaterial, std::vector<Entity>* entityList);

	void SpawnNewCar(PxVec3 spawnPosition, PxQuat spawnRotation);

	//need to figure out where to respawn car (WIP)
	void RespawnCar(EngineDriveVehicle* carToRespawn);

	EngineDriveVehicle* GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic);

	//MIGHT SHIFT OUT TO ANOTHER CLASS IF I CAN FIND A GOOD PLACE FOR A HELPER CLASS
	Entity* GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic);

	std::vector<EngineDriveVehicle*> GetGVehicleList();

	/*
	* PROJECTILES
	*/

	//constants
	const PxReal projectileMass = 10.0f;
	const PxReal projectileRadius = 1.0f;
	const float shootForce = 100;

	//the dictionary for all projectiles for all cars
	std::map<EngineDriveVehicle*, std::vector<PxRigidDynamic*>> projectileRigidDynamicDict;

	void Shoot(EngineDriveVehicle* shootingCar);

	void DestroyProjectile(PxRigidDynamic* projectileToDestroy);

	//TODO: might need a getter for finding the projectile for collisions
};