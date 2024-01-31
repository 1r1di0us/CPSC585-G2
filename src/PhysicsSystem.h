#pragma once
#include "PxPhysicsAPI.h"
#include "Entity.h"
#include "Transform.h"
#include "Model.h"
#include <vector>
#include <iostream>

//vehicle required includes. using four wheel drive model
#include <ctype.h>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

//VEHICLE STUFF
using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

//Commands are issued to the vehicle in a pre-choreographed sequence.
struct Command
{
	PxF32 brake;
	PxF32 throttle;
	PxF32 steer;
	PxU32 gear;
	PxF32 duration;
};

//END VEHICLE STUFF

class PhysicsSystem {

public:

	//timestep value, easily modifiable
	const PxReal TIMESTEP = 1.0f / 60.0f;

	//VEHICLE STUFF

	//PhysX management class instances.
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	PxPvd* gPvd = NULL;

	//The path to the vehicle json files to be loaded.
	const char* gVehicleDataPath = "assets/vehicledata";

	//The vehicle with engine drivetrain
	EngineDriveVehicle gVehicle;

	//Vehicle simulation needs a simulation context
	//to store global parameters of the simulation such as 
	//gravitational acceleration.
	PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	//Gravitational acceleration
	const PxVec3 gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
	
	//I have a feeling this will be the key to having controls for car
	Command gCommands[5] =
	{
		{0.5f, 0.0f, 0.0f, gTargetGearCommand, 2.0f},	//brake on and come to rest for 2 seconds
		{0.0f, 0.65f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
		{0.5f, 0.0f, 0.0f, gTargetGearCommand, 5.0f},	//brake for 5 seconds
		{0.0f, 0.75f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
		{0.0f, 0.25f, 0.5f, gTargetGearCommand, 5.0f}	//light throttle and steer for 5 seconds.
	};
	const PxU32 gNbCommands = sizeof(gCommands) / sizeof(Command);
	PxReal gCommandTime = 0.0f;			//Time spent on current command
	PxU32 gCommandProgress = 0;			//The id of the current command.

	//A ground plane to drive on.
	PxRigidStatic* gGroundPlane = NULL;

	//END VEHICLE STUFF

	physx::PxVec3 getPos(int i);

	void updateTransforms();

	//physx basic setup
	void initPhysX();
	void initGroundPlane();
	void initMaterialFrictionTable();
	bool initVehicle(PxVec3 spawnPosition, PxQuat spawnRotation, const char vehicleName[]);

	void createBoxes();

	void stepPhysics();

	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;

	PhysicsSystem(); // Constructor

	void cleanPhysicsSystem();
};