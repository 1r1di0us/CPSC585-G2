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

//respawn square struct
struct MapSquare {
	int id;
	int numPoints;
	std::vector<PxVec2> pointsInIt;
	PxVec2 bottomLeft;
	PxVec2 topRight;
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

	//map coords for the corners
	const PxVec2 BOTTOMLEFTMAPCOORD = PxVec2(-10.0f, -10.0f);
	const PxVec2 TOPRIGHTMAPCOORD = PxVec2(10.0f, 10.0f);
	
	//the approximate size of the map. rectangular
	const PxReal MAPLENGTHX = TOPRIGHTMAPCOORD.x - BOTTOMLEFTMAPCOORD.x;
	const PxReal MAPLENGTHZ = TOPRIGHTMAPCOORD.y - BOTTOMLEFTMAPCOORD.y;

	//the min distance cars can spawn from other cars
	const PxReal CARMINSPAWNDISTANCE = 3.0f;

	//the min spawn distance between powerups
	const PxReal POWERUPSPAWNMINDISTANCE = 2.0f;

	//the spawn height of cars (looks cool, but careful of flipping)
	const PxReal CARSPAWNHEIGHT = 0.5f;

	//the spawn height of powerups
	const PxReal POWERUPSPAWNHEIGHT = 0.0f;

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

	//for respawning, the list of map squares
	std::vector<MapSquare> mapSquareList;

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

	//gets rid of the y
	PxVec2 ConvertVec3ToVec2(PxVec3 vectorToConvert);

	//determines if a given point is in a square
	bool IsPointInSquare(PxVec2 point, MapSquare& square);

	//function to get the x nearest points given a list of points
	std::vector<PxVec2> GetXNearestPoints(std::vector<PxVec2> pointList, int numPointsToGet, std::vector<PxVec2> pointsOfSameType);

	//finds the center of four points
	PxVec2 FindCenterOfFourPoints(std::vector<PxVec2> pointsList);

	void PopulateMapSquareList(std::vector<PxVec2> pointsOfSameType);

	//generates a point a min distance away from all points in given vec and within the map range
	PxVec3 GenerateSpawnPoint(std::vector<PxVec2> pointsOfSameType, PxReal minDistance, PxReal spawnHeight);

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