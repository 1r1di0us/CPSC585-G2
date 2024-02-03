#pragma once
#include <PxPhysicsAPI.h>
#include "snippetvehicle2common/SnippetVehicleHelpers.h"

using namespace physx;

class Projectile {

public:

	PxReal mass = 10.0f;

	PxRigidDynamic* body;

	//constructor to make a projectile
	Projectile(PxPhysics* gPhysics, PxScene* gScene, PxReal radius, PxMaterial* gMaterial, PxVec3 spawnPosition);

	//gives the projectile a force in a direction
	void shootProjectile(PxVec3 force);
};