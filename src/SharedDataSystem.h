#pragma once

#include <vector>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"
#include "Entity.h"
#include <unordered_map>
#include <memory>

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

//car info struct
struct CarInfo{
	//waow smart pointer! I HARDLY KNOW ER
	std::shared_ptr<Entity> entity;
	int score = 0;
	//THANKS MURTAZA!
	bool isAlive = true;
	float respawnTimeLeft = 0;
	float parryTimeLeft = 0;
};

class SharedDataSystem {

private:
	//custom collision callback system
	class ContactReportCallback : public PxSimulationEventCallback {

		public:

			//THANKS MATT!
			bool contactDetected = false;
			PxContactPairHeader contactPair;

		private:
			void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
				//PX_UNUSED(pairHeader);
				//PX_UNUSED(pairs);
				PX_UNUSED(nbPairs);

				//call the resolver here to deal with more than one collision pair per physics sim frame?
				
				contactPair = pairHeader;
				if (pairHeader.pairs->events.isSet(PxPairFlag::eNOTIFY_TOUCH_FOUND))
					contactDetected = true;
			}
			void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
			void onWake(physx::PxActor** actors, physx::PxU32 count) {}
			void onSleep(physx::PxActor** actors, physx::PxU32 count) {}
			void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {}
			void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
				const physx::PxTransform* poseBuffer,
				const physx::PxU32 count) {}
	};

public:

	/*
	* CONSTANTS:
	*/

	//respawn timer
	const float RESPAWNLENGTH = 2.5f;

	//timestep value, easily modifiable
	const PxReal TIMESTEP = 1.0f / 60.0f;

	//the min distance cars can spawn from other cars
	const PxReal CARSPAWNDISTANCE = 5.0f;

	//entity helper functions move from entity cpp?

	//the GOAT list of entities
	std::vector<Entity> entityList;

	ContactReportCallback* gContactReportCallback = new ContactReportCallback();

	//PhysX management class instances.
	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;

	//need to have list of rigid dynamics corresponding to gvehicles vehicles to move the correct vehicle given rigid dynamic
	std::vector<PxRigidDynamic*> carRigidDynamicList;
	std::vector<EngineDriveVehicle*> gVehicleList;
	
	//a vector of all car structs for car info
	std::vector<CarInfo> carInfoList;

	//gets the car info struct using an entity
	CarInfo* GetCarInfoStructFromEntity(std::shared_ptr<Entity> entity);

	//gets the gVehicle given the rigid dynamic
	EngineDriveVehicle* GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic);

	//gets the rigid dynamic from the vehicle
	PxRigidDynamic* GetRigidDynamicFromVehicle(EngineDriveVehicle* gVehicle);

	//returns an entity given its rigid dynamic component
	std::shared_ptr<Entity> GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic);

	//gets the list of dead cars to do shit to
	std::vector<CarInfo*> GetListOfDeadCars();

	//returns a location where an entity can be respawned
	PxVec3 DetermineSpawnLocation(PhysicsType physType);

	/*
	* PROJECTILES
	*/

	//the dictionary for all projectiles for all cars
	std::unordered_map<PxRigidDynamic*, std::vector<PxRigidDynamic*>> carProjectileRigidDynamicDict;

	//finds the car that shot a given projectile
	std::shared_ptr<Entity> GetCarThatShotProjectile(PxRigidDynamic* projectile);

	//collision logic functions
	void CarProjectileCollisionLogic(PxActor* car, PxActor* projectile);
	void CarPowerupCollisionLogic(PxActor* car, PxActor* powerup);

	//THIS MAY NOT WORK IN THE SWITCH DEPENDING ON HOW THE MAP EXISTS
	void ProjectileStaticCollisionLogic(PxActor* projectile);

	//function to resolve all collisions
	/*
	* Resolves:
	* Projectile + Car
	* Projectile + Map
	* Car + Powerup
	*/
	void ResolveCollisions();

};