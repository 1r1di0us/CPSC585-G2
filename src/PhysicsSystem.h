#pragma once
#include "PxPhysicsAPI.h"
#include "Entity.h"  
#include "Car.h"
#include <vector>
#include <iostream>

using namespace physx;
using namespace physx::vehicle2;

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

	std::vector<physx::PxRigidDynamic*> rigidDynamicList;
	std::vector<Transform*> transformList;

	//Vehicle simulation needs a simulation context
	//to store global parameters of the simulation such as 
	//gravitational acceleration.
	PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	physx::PxVec3 getPos(int i);

	void updateTransforms();

	//physx basic setup
	void initPhysX();
	void initGroundPlane();
	void initMaterialFrictionTable();
	void createBoxes();

	void stepAllVehicleMovementPhysics(std::vector<Car*> carList);

	void stepPhysics(std::vector<Entity> entityList);

	PhysicsSystem(); // Constructor

	void cleanPhysicsSystem();

	PxPhysics* getPhysics();

	PxScene* getScene();

	PxVec3 getGravity();

	PxMaterial* getMaterial();
};