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

	//I have a feeling this will be the key to having controls for car
	//this is the array of commands to execute for this car. need to link this to input system
	std::vector<Command> gCommands =
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

	//the transform of the car
	PxTransform carTransform;

	//gets the depth of the bounding box for this vehicle
	PxVec3 vehicleBodyDimensions;

	//function outlines
	Car(const char* name, PxVec3 spawnPosition, PxQuat spawnRotation, PxPhysics* gPhysics, PxScene* gScene, PxVec3 gGravity, PxMaterial* gMaterial);

	void DestroyCar();

	//TODO: potentially rework to just a setter for car direction, depending on if need more vars here
	void setCarTransform();

};