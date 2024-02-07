#pragma once
#include <PxPhysicsAPI.h>
#include "snippetvehicle2common/SnippetVehicleHelpers.h"

using namespace physx;

class Projectile {

public:

	//constants
	const PxReal mass = 10.0f;
	const PxReal radius = 1.0f;
	const float shootForce = 100;

	PxRigidDynamic* body;

	//constructor to make a projectile
	Projectile(PxPhysics* gPhysics, PxScene* gScene, PxMaterial* gMaterial, PxTransform spawnPosition);

	//gives the projectile a force in a direction
	void shootProjectile(PxVec3 directionToShoot);
};