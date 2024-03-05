#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"
#include "Entity.h"
#include <unordered_map>
#include <memory>
#include <chrono>
#include <queue>
#include "math.h"

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

//needs to be here cause the struct wont see it below
const int AMMO_START_AMOUNT = 5;

//car info struct
struct CarInfo{
	//waow smart pointer! I HARDLY KNOW ER
	std::shared_ptr<Entity> entity;
	int score = 0;
	//THANKS MURTAZA!
	bool isAlive = true;
	float respawnTimeLeft = 0;
	float parryTimeLeft = 0;
	int ammoCount = AMMO_START_AMOUNT;
};

//powerup types
enum PowerupType {
	AMMO,
	PROJECTILESPEED,
	PROJECTILESIZE,
	CARSPEED, 
	
	//used for generating random powerup type
		//set to 1 for now cause only ammo powerup exists
	NUM_POWERUP_TYPES = 1
};

//powerup info struct
struct PowerupInfo {
	std::shared_ptr<Entity> entity;
	PowerupType powerupType;
};

//respawn square struct
struct MapSquare {
	int id;
	int numPoints = 0;
	std::vector<PxVec2> pointsInIt;
	PxVec2 bottomLeft;
	PxVec2 topRight;
};

// Comparator for priority queue based on distance
struct CompareDistance {
	bool operator()(const std::pair<float, PxVec2>& p1, const std::pair<float, PxVec2>& p2) {
		return p1.first < p2.first;
	}
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
			void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {
			
				//used for trigger volumes
				contactPair.actors[0] = pairs->triggerActor;
				contactPair.actors[1] = pairs->otherActor;

				if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
					contactDetected = true;

			}
			void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
				const physx::PxTransform* poseBuffer,
				const physx::PxU32 count) {}
	};

	/*
	* CAR RESPAWN FUNCTIONS
	*/

	//gets rid of the y
	PxVec2 ConvertVec3ToVec2(PxVec3 vectorToConvert);

	//determines if a given point is in a square
	bool IsPointInSquare(PxVec2 point, MapSquare& square);

	//checks if the point is in the world bounds
	bool IsPointInBounds(PxVec2 point);

	// Function to calculate the distance between two PhysX vectors
	float DistanceBetweenPoints(const PxVec2& point1, const PxVec2& point2);

	//function to get the x nearest points given a list of points
	std::vector<PxVec2> GetXNearestPoints(std::vector<PxVec2> pointList, int numPointsToGet, std::vector<PxVec2> pointsOfSameType);

	//finds the center of four points
	PxVec2 FindCenterOfFourPointsWithRandomOffset(PxReal minDistance, std::vector<PxVec2> existingPointsList = {}, std::vector<PxVec2> generatedPointsList = {});

	//populates the map square list with the points of the same type
	void PopulateMapSquareList(std::vector<PxVec2> pointsOfSameType, std::vector<MapSquare>& mapSquareList);

	//randomizes the map square list
	void RandomizeMapSquareList(std::vector<MapSquare>& mapSquareList);

	//creates the map square list
	void CreateMapSquareList();

	//generates a point a min distance away from all points in given vec and within the map range
	PxVec3 GenerateSpawnPoint(std::vector<PxVec2> pointsOfSameType, PxReal minDistance, PxReal spawnHeight);


public:

	/*
	* CONSTANTS:
	*/

	//timestep value, easily modifiable
	const PxReal TIMESTEP = 1.0f / 60.0f;

	//car respawn timer
	const float CAR_RESPAWN_LENGTH = 3.0f;

	//map coords for the corners
	const PxVec2 BOTTOM_LEFT_MAP_COORD = PxVec2(-20.0f, -20.0f);
	const PxVec2 TOP_RIGHT_MAP_COORD = PxVec2(20.0f, 20.0f);
	
	//the approximate size of the map. rectangular
	const PxReal MAPLENGTHX = TOP_RIGHT_MAP_COORD.x - BOTTOM_LEFT_MAP_COORD.x;
	const PxReal MAPLENGTHZ = TOP_RIGHT_MAP_COORD.y - BOTTOM_LEFT_MAP_COORD.y;

	//the min distance cars can spawn from other cars
	const PxReal CAR_MIN_SPAWN_DISTANCE = 15.0f;

	//the min spawn distance between powerups
	const PxReal POWERUP_MIN_SPAWN_DISTANCE = 10.0f;

	//the spawn height of cars
	const PxReal CAR_SPAWN_HEIGHT = 0.5f;

	//the spawn height of powerups
	const PxReal POWERUP_SPAWN_HEIGHT = 0.0f;

	//the spawn rate of a random powerup
	const float RANDOM_POWERUP_SPAWN_RATE = 50.0f;

	//the spawn rate of an ammo spawn powerup
	const float AMMO_POWERUP_SPAWN_RATE = 10.0f;

	//the max number of ammo spawn powerups
	const int NUMBER_OF_AMMO_POWERUPS = 3;

	//the number of bullets given per ammo powerup
	const int NUMBER_AMMO_GIVEN_PER_POWERUP = 2;

	//entity helper functions move from entity cpp?

	//the GOAT list of entities
	std::vector<Entity> entityList;

	//custom collision instance
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

	/*
	* DEBUGGING STUFF
	*/

	//will make debug boxes at all the squares used in respawning
	bool boxesMade = false;

	//makes a floating box for boundary demo purposes
	void MAKE_BOX_DEBUG(PxReal x, PxReal z);

	/*
	* CAR FUNCTIONS
	*/

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
	PxVec3 DetermineRespawnLocation(PhysicsType physType);

	/*
	* PROJECTILES
	*/

	//the dictionary for all projectiles for all cars
	std::unordered_map<PxRigidDynamic*, std::vector<PxRigidDynamic*>> carProjectileRigidDynamicDict;

	//finds the car that shot a given projectile
	std::shared_ptr<Entity> GetCarThatShotProjectile(PxRigidDynamic* projectile);

	/*
	* POWERUPS
	*/

	//the modifiable time variables for respawning
	float timeUntilRandomPowerup = RANDOM_POWERUP_SPAWN_RATE;
	float timeUntilAmmoPowerup = AMMO_POWERUP_SPAWN_RATE;

	//list of all powerups on map
	std::vector<PowerupInfo> allPowerupList;

	//an int to make the powerup names unique (cant do it based on list size)
	int spawnedPowerupCounter = 0;

	//function to find a powerup info by entity
	PowerupInfo* GetPowerupInfoStructFromEntity(std::shared_ptr<Entity> entity);

	/*
	* COLLISIONS
	*/

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

	// Delete all lists in SharedDataSystem.h
	void resetSharedDataSystem();

	// Stuff moved in from GameState.cpp
	void menuEventHandler();

	//makes the rotation matrix for the camera
	glm::mat3 getCamRotMat();
	PxMat33 getCamRotMatPx(float angle);

	// Flags
	bool inMenu = true;
	bool inControlsMenu = false;
	bool loading = false;
	bool quit = false;
	bool inResults = false;
	int menuOptionIndex = 0;
	int nbMenuOptions = 3; // Currently options are play and quit

	bool inGameMenu = false;
	int ingameOptionIndex = 0;
	int nbIngameOptions = 2; // Options will be main menu and quit

	// Reset these on game end
	int winningPlayer = 0;
	bool tieGame = false;
	bool carsInitialized = false;
	bool menuMusicPlaying = false;
	bool gameMusicPlaying = false;
	bool resultsMusicPlaying = false;

	//// Audio 
	//AudioManager* audio_ptr = nullptr;
	//glm::vec3 listener_position;

	// Camera
	float cameraAngle = M_PI;
	int useBirdsEyeView = 0;
};