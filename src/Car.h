#pragma once
#include <vector>
#include <string>

//vehicle required includes. using four wheel drive model
#include <ctype.h>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

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

class Car{

public:

	/*
	* SHARED INFORMATION
	*/

	PxPhysics* gPhysics;
	PxScene* gScene;
	PxMaterial* gMaterial;

	/*
	* PROJECTILE PORTION
	*/

	//constants
	const PxReal projectileMass = 10.0f;
	const PxReal projectileRadius = 1.0f;
	const float shootForce = 100;

	//needs to be a rigid dynamic list as opposed to a transform list for easier deleting (i think)
	//will be used to update projectile transforms
	std::vector<PxRigidDynamic*> projectileBodyList;

	//creates and shoots a projectile
	void shootProjectile();

	/*
	* CAR PORTION
	*/

	//The path to the vehicle json files to be loaded.
	const char* gVehicleDataPath = "assets/vehicledata";

	//The vehicle with engine drivetrain
	EngineDriveVehicle gVehicle;

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	//automatic transmission
	const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//the transform of the car
	PxTransform carTransform;

	//gets the depth of the bounding box for this vehicle
	PxVec3 vehicleBodyDimensions;

	//function outlines
	Car(const char* name, PxVec3 spawnPosition, PxQuat spawnRotation, PxPhysics* gPhysics, PxScene* gScene, PxVec3 gGravity, PxMaterial* gMaterial);

	void DestroyCar();

	//updates car transform
	void setCarTransform();
};