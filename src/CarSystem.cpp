#include "CarSystem.h"

CarSystem::CarSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;
}

void CarSystem::SpawnNewCar(PxVec2 spawnPosition, PxQuat spawnRotation) {

	//every car has same name TODO: might change if need to sort by name
	const char* name = "car";

	//The vehicle with engine drivetrain
	EngineDriveVehicle* gVehicle = new EngineDriveVehicle();

	//Load the params from json or set directly.
	readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle->mBaseParams);
	setPhysXIntegrationParams(gVehicle->mBaseParams.axleDescription,
		gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
		gVehicle->mPhysXParams);
	readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json",
		gVehicle->mEngineDriveParams);

	//Set the states to default.
	if (!gVehicle->initialize(*dataSys->gPhysics, PxCookingParams(PxTolerancesScale()), *dataSys->gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE)) {
		printf("Car initialization failed\n");
		exit(69);
	}

	//Apply a start pose to the physx actor and add it to the physx scene.
	PxTransform carTransform = PxTransform(PxVec3(spawnPosition.x, dataSys->CAR_SPAWN_HEIGHT, spawnPosition.y), spawnRotation);
	gVehicle->setUpActor(*dataSys->gScene, carTransform, name);

	PxFilterData vehicleFilter(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	//i no longer have access to this list in here
	//rigidDynamicList.push_back(gVehicle.mPhysXState.physxActor.rigidBody);

	//TODO: look at shapes and adding to rigid dynamic list

	//making all parts of the vehicle have collisions with the outside world
	PxU32 shapes = gVehicle->mPhysXState.physxActor.rigidBody->getNbShapes();
	for (PxU32 i = 0; i < shapes; i++) {
		PxShape* shape = NULL;
		gVehicle->mPhysXState.physxActor.rigidBody->getShapes(&shape, 1, i);

		//the body of the vehicle is at i = 0
		//TODO: commented out for now but might need to work around if need these
		/*if (i == 0) {
			PxGeometryHolder carChassis = shape->getGeometry();
			vehicleBodyDimensions = carChassis.box().halfExtents * 2;
		}*/

		shape->setSimulationFilterData(vehicleFilter);

		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	//Set the vehicle in 1st gear.
	gVehicle->mEngineDriveState.gearboxState.currentGear = gVehicle->mEngineDriveParams.gearBoxParams.neutralGear + 1;
	gVehicle->mEngineDriveState.gearboxState.targetGear = gVehicle->mEngineDriveParams.gearBoxParams.neutralGear + 1;

	//Set the vehicle to use the automatic gearbox.
	//gVehicle->mTransmissionCommandState.targetGear = gVehicle->mTransmissionCommandState.eAUTOMATIC_GEAR;
	//set the vehicle to be in 1st gear
	gVehicle->mTransmissionCommandState.targetGear = 2;

	//adding car to needed lists
	dataSys->carRigidDynamicList.emplace_back((PxRigidDynamic*)gVehicle->mPhysXState.physxActor.rigidBody);
	dataSys->gVehicleList.emplace_back(gVehicle);
	dataSys->carProjectileRigidDynamicDict[(PxRigidDynamic*)gVehicle->mPhysXState.physxActor.rigidBody] = std::vector<PxRigidDynamic*>();

	//creating the car entity to add to the entity list
	Entity car;
	car.name = "car" + std::to_string(dataSys->gVehicleList.size());
	car.CreateTransformFromPhysX(gVehicle->mPhysXState.physxActor.rigidBody->getGlobalPose());
	car.physType = PhysicsType::CAR;
	car.collisionBox = dataSys->carRigidDynamicList.back();

	dataSys->entityList.emplace_back(car);

	//creating the car info struct
	CarInfo carInfo;
	carInfo.entity = std::make_shared<Entity>(dataSys->entityList.back());
	carInfo.shootDir = car.collisionBox->getGlobalPose().q.getBasisVector2();
	
	dataSys->carInfoList.emplace_back(carInfo);

}

void CarSystem::RespawnAllCars() {

	//go through all dead cars
	for (CarInfo* carInfo : dataSys->GetListOfDeadCars()) {

		//if the car is ready to be respawned
		if (carInfo->respawnTimeLeft <= 0) {

			//get the spawn location
			PxVec3 spawnVec = dataSys->DetermineRespawnLocation(carInfo->entity->physType);

			//"spawn" the car
			carInfo->isAlive = true;
			carInfo->respawnTimeLeft = 0;
			carInfo->ammoCount = AMMO_START_AMOUNT;
			carInfo->entity->collisionBox->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, false);
			carInfo->entity->collisionBox->setGlobalPose(PxTransform(spawnVec));
		}
		else {

			//subtract the physics system update rate from the respawn timer
				//real time instead maybe?
			carInfo->respawnTimeLeft -= dataSys->TIMESTEP;
		}
	}

}

bool CarSystem::Shoot(PxRigidDynamic* shootingCar) {

	//if the car is dead, it cant shoot
	if (dataSys->DEBUG_PRINTS) printf("before shoot 1\n");
	if (!dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->isAlive) {
		if (dataSys->DEBUG_PRINTS) printf("after shoot 1\n");
		return false;
	}

	if (dataSys->DEBUG_PRINTS) printf("before shoot 2\n");
	//if the car has no ammo it can't shoot
	if (dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->ammoCount <= 0) {
		if (dataSys->DEBUG_PRINTS) printf("after shoot 2\n");
		return false;
	}

	//car info struct
	CarInfo* carInfo = dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar));

	//calculating the actual projectile radius, taking powerups into account
	float actualProjectileRadius = dataSys->DEFAULT_PROJECTILE_RADIUS;

	//projectile radius powerup
	if (dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->projectileSizeActiveTimeLeft > 0) {

		actualProjectileRadius *= dataSys->PROJECTILE_SIZE_POWERUP_STRENGTH;
	}

	//creating the projectile to shoot
	//it is offset based on the radius of the projectile
	PxTransform spawnTransform = PxTransform(
		PxVec3(shootingCar->getGlobalPose().p.x + carInfo->shootDir.x * actualProjectileRadius * 6.5,
			actualProjectileRadius * 2,
			shootingCar->getGlobalPose().p.z + carInfo->shootDir.z * actualProjectileRadius * 6.5),
		shootingCar->getGlobalPose().q);

	//define a projectile
	physx::PxShape* shape = dataSys->gPhysics->createShape(physx::PxSphereGeometry(actualProjectileRadius), *dataSys->gMaterial);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_PROJECTILE, COLLISION_FLAG_PROJECTILE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	//creates the rigid dynamic body to be a diff instance for each projectile (cant be sharing that)
	PxRigidDynamic* projectileBody = dataSys->gPhysics->createRigidDynamic(spawnTransform);

	projectileBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*projectileBody, projectileMass);

	//disables gravity for the projectile
	projectileBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	//adds the projectile to the scene
	dataSys->gScene->addActor(*projectileBody);

	//if the car shooting has the projectile speed powerup
	if (dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->projectileSpeedActiveTimeLeft > 0) {

		//sets the linear velocity of the projectile (ignores y direction of car cause it wiggles)
		projectileBody->setLinearVelocity(dataSys->SHOOT_FORCE * dataSys->PROJECTILE_SPEED_POWERUP_STRENGTH * PxVec3(carInfo->shootDir.x, 0, carInfo->shootDir.z));
	}
	else {

		//sets the linear velocity of the projectile (ignores y direction of car cause it wiggles)
		projectileBody->setLinearVelocity(dataSys->SHOOT_FORCE * PxVec3(carInfo->shootDir.x, 0, carInfo->shootDir.z));
	}

	//adding the projectile to the dict for the correct car
	dataSys->carProjectileRigidDynamicDict[shootingCar].emplace_back(projectileBody);

	if (dataSys->DEBUG_PRINTS) printf("before projectile entity creation\n");

	//creating the projectile entity with name based on car that shot it
	Entity projectile;
	projectile.name = dataSys->GetEntityFromRigidDynamic(shootingCar)->name + "projectile" + std::to_string(dataSys->spawnedProjectileCounter++);
	projectile.CreateTransformFromPhysX(projectileBody->getGlobalPose());
	projectile.physType = PhysicsType::PROJECTILE;
	projectile.collisionBox = projectileBody;
	
	if (dataSys->DEBUG_PRINTS) printf("after projectile entity creation\n");

	//makes its name smart for easy debugging
	projectileBody->setName(projectile.name.c_str());

	dataSys->entityList.emplace_back(projectile);

	if (dataSys->DEBUG_PRINTS) printf("before reduce ammo count\n");

	//subtract one ammo from the count
	dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->ammoCount--;

	if (dataSys->DEBUG_PRINTS) printf("after reduce ammo count\n");

	return true;

}

void CarSystem::UpdateAllCarCooldowns() {

	//updates all the car cooldowns
	for (int i = 0; i < dataSys->carInfoList.size(); i++) {

		//edits correct cooldown based on if the parry is active or not
		if (dataSys->carInfoList[i].parryActiveTimeLeft > 0) {

			dataSys->carInfoList[i].parryActiveTimeLeft -= dataSys->TIMESTEP;
		}
		else {

			dataSys->carInfoList[i].parryCooldownTimeLeft -= dataSys->TIMESTEP;
		}

		//the projectile speed powerup
		if (dataSys->carInfoList[i].projectileSpeedActiveTimeLeft > 0)
			dataSys->carInfoList[i].projectileSpeedActiveTimeLeft -= dataSys->TIMESTEP;
		
		//the projectile size powerup
		if (dataSys->carInfoList[i].projectileSizeActiveTimeLeft > 0)
			dataSys->carInfoList[i].projectileSizeActiveTimeLeft -= dataSys->TIMESTEP;
	}
}