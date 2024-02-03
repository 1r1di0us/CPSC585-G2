#include "Projectile.h"
#include <PxPhysicsAPI.h>

Projectile::Projectile(physx::PxPhysics* gPhysics, physx::PxReal radius, physx::PxReal halfHeight, physx::PxMaterial* gMaterial) {

	physx::PxShape* shape = gPhysics->createShape(physx::PxCapsuleGeometry(radius, halfHeight), *gMaterial);

}

void Projectile::pushProjectile(physx::PxVec3 force) {


}
