#pragma once

#include <vector>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

class CarSystem {

public:

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxMaterial* gMaterial;

	//The path to the vehicle json files to be loaded.
	const char* gVehicleDataPath = "assets/vehicledata";

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	//automatic transmission
	const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//need to have list of rigid dynamics corresponding to gvehicles vehicles to move the correct vehicle given rigid dynamic
	std::vector<PxRigidDynamic*> rigidDynamicList;
	std::vector<EngineDriveVehicle*> gVehicleList;

	//constructor
	CarSystem(PxPhysics* gPhysics, PxScene* gScene, PxMaterial* gMaterial);

	void SpawnNewCar(PxVec3 spawnPosition, PxQuat spawnRotation);

	void UpdateAllCars();

	void DestroyCar();

};