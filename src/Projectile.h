#pragma once
#include <PxPhysicsAPI.h>
#include "snippetvehicle2common/SnippetVehicleHelpers.h"

using namespace physx;

class Projectile {

public:

	physx::PxReal mass = 10.0f;

	physx::PxRigidDynamic* body;

	//constructor to make a projectile
	Projectile(physx::PxPhysics* gPhysics, physx::PxScene* gScene, physx::PxReal radius, physx::PxMaterial* gMaterial, physx::PxVec3 spawnPosition);

	//gives the projectile a force in a direction
	void pushProjectile(physx::PxVec3 force);
};