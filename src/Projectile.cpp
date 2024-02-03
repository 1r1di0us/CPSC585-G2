#include "Projectile.h"

Projectile::Projectile(physx::PxPhysics* gPhysics, physx::PxScene* gScene, physx::PxReal radius, physx::PxMaterial* gMaterial, physx::PxVec3 spawnPosition) {

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxSphereGeometry(radius), *gMaterial);
	physx::PxTransform tran(spawnPosition);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	body = gPhysics->createRigidDynamic(tran);

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);

	//disables gravity for the projectile
	body->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	gScene->addActor(*body);

}

void Projectile::shootProjectile(PxVec3 force) {

	body->setLinearVelocity(force);
}