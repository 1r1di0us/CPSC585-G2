#include "Projectile.h"

Projectile::Projectile(physx::PxPhysics* gPhysics, physx::PxScene* gScene, physx::PxReal radius, physx::PxMaterial* gMaterial, physx::PxVec3 spawnPosition) {

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxCapsuleGeometry(radius, 0.0f), *gMaterial);
	physx::PxTransform tran(spawnPosition);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	body = gPhysics->createRigidDynamic(tran);

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);
	gScene->addActor(*body);

}

void Projectile::pushProjectile(physx::PxVec3 force) {

}