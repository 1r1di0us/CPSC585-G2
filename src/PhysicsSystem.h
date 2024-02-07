#pragma once
#include "PxPhysicsAPI.h"
#include "Entity.h"
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

	//lists for all differentTypes of components (I HATE THIS, BUT CANT GIVE IT ENTITY LIST CAUSE THAT BREAKS ECS)
	std::vector<Transform*> carTransformList;
	std::vector<Transform*> projectileTransformList;
	std::vector<Projectile*> projectileList;
	//car list for efficiency
	std::vector<Car*> carList;

	//Vehicle simulation needs a simulation context
	//to store global parameters of the simulation such as 
	//gravitational acceleration.
	PxVehiclePhysXSimulationContext gVehicleSimulationContext;

	void updateTransforms();

	//physx basic setup
	void initPhysX();
	void initGroundPlane();
	void initMaterialFrictionTable();
	void initVehicleSimContext();

	void stepAllVehicleMovementPhysics(std::vector<Car*> carList);

	void stepPhysics(std::vector<Entity> entityList);

	//the projectile has a spawn position by default
	void shootProjectile(Entity* car, Entity* projectileToShoot);

	PhysicsSystem(); // Constructor

	void cleanPhysicsSystem();

	PxPhysics* getPhysics();

	PxScene* getScene();

	PxVec3 getGravity();

	PxMaterial* getMaterial();
};