#pragma once
#include "PxPhysicsAPI.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"
#include "Entity.h"
#include <vector>
#include <cmath>

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

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

	//Gravitational acceleration
	const PxVec3 gGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	//The mapping between PxMaterial and friction.
	PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
	PxU32 gNbPhysXMaterialFrictions = 0;
	PxReal gPhysXDefaultMaterialFriction = 1.0f;

	//A ground plane to drive on.
	PxRigidStatic* gGroundPlane = NULL;

	//Vehicle simulation needs a simulation context
	//to store global parameters of the simulation such as 
	//gravitational acceleration.
	PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	//physx basic setup
	void initPhysX();
	void initGroundPlane();
	void initMaterialFrictionTable();
	void initVehicleSimContext();

	void stepAllVehicleMovementPhysics(std::vector<EngineDriveVehicle*> carList);

	void stepPhysics(std::vector<Entity> entityList, std::vector<EngineDriveVehicle*> carList);

	PhysicsSystem(); // Constructor

	void cleanPhysicsSystem();

	PxPhysics* getPhysics();

	PxScene* getScene();

	PxVec3 getGravity();

	PxMaterial* getMaterial();

	double getTIMESTEP();
};