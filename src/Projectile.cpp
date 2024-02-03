#include "Projectile.h"

Projectile::Projectile(physx::PxPhysics* gPhysics, physx::PxScene* gScene, physx::PxReal radius, physx::PxReal halfHeight, physx::PxMaterial* gMaterial, physx::PxVec3 spawnPosition) {

	//physx::PxShape* shape = gPhysics->createShape(physx::PxCapsuleGeometry(radius, halfHeight), *gMaterial);

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(radius, radius, radius), *gMaterial);
	physx::PxTransform tran(spawnPosition);

	//creating collision flags for each projectile
	physx::PxFilterData boxFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(boxFilter);

	body = gPhysics->createRigidDynamic(tran);

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);
	gScene->addActor(*body);

}

void Projectile::pushProjectile(physx::PxVec3 force) {

}