#pragma once
#include "PxPhysicsAPI.h"
#include "Entity.h"
#include "Transform.h"
#include "Model.h"
#include <vector>
#include <iostream>



class PhysicsSystem {

public:
	physx::PxVec3 getPos(int i);

	void updateTransforms();

	std::vector<physx::PxRigidDynamic*> rigidDynamicList; // make sure to add this!
	std::vector<Transform*> transformList;

	//PhysX management class instances.
	physx::PxDefaultAllocator gAllocator;
	physx::PxDefaultErrorCallback gErrorCallback;
	physx::PxFoundation* gFoundation = NULL;
	physx::PxPhysics* gPhysics = NULL;
	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene* gScene = NULL;
	physx::PxMaterial* gMaterial = NULL;
	physx::PxPvd* gPvd = NULL;

	PhysicsSystem(); // Constructor
};