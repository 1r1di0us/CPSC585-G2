#include "Projectile.h"

Projectile::Projectile(PxPhysics* gPhysics, PxScene* gScene, PxMaterial* gMaterial, PxTransform spawnPosition) {

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxSphereGeometry(radius), *gMaterial);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	body = gPhysics->createRigidDynamic(spawnPosition);

	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);

	//disables gravity for the projectile
	body->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	gScene->addActor(*body);

}

void Projectile::shootProjectile() {

	body->setLinearVelocity(shootForce);
}