#pragma once

class Projectile {

public:

	//constructor to make a projectile
	Projectile(physx::PxPhysics* gPhysics, physx::PxReal radius, physx::PxReal halfHeight, physx::PxMaterial* gMaterial);

	//gives the projectile a force in a direction
	void pushProjectile(physx::PxVec3 force);
};