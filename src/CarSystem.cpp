#include "CarSystem.h"

CarSystem::CarSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;
}

void CarSystem::SpawnNewCar(PxVec3 spawnPosition, PxQuat spawnRotation) {

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
	PxTransform carTransform = PxTransform(spawnPosition, spawnRotation);
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

void CarSystem::Shoot(PxRigidDynamic* shootingCar) {

	//if the car is dead, it cant shoot
	if (!dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->isAlive) {
		return;
	}

	//if the car has no ammo it can't shoot
	if (dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->ammoCount <= 0) {
		return;
	}

	//gets the forward vector of the car
	PxVec3 forwardVector = shootingCar->getGlobalPose().q.getBasisVector2();

	//creating the projectile to shoot
	//it is offset based on the radius of the projectile
	//TODO: THIS WILL BE REWORKED WHEN SPAWNING PROJECTILE BASED ON CAMERA DIRECTION AND TURRET SIZE
	PxTransform spawnTransform = PxTransform(
		PxVec3(shootingCar->getGlobalPose().p.x + forwardVector.x * 5,
			shootingCar->getGlobalPose().p.y + projectileRadius + 0.1f,
			shootingCar->getGlobalPose().p.z + forwardVector.z * 5),
		shootingCar->getGlobalPose().q);

	//define a projectile
	physx::PxShape* shape = dataSys->gPhysics->createShape(physx::PxSphereGeometry(projectileRadius), *dataSys->gMaterial);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_PROJECTILE, COLLISION_FLAG_PROJECTILE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	//creates the rigid dynamic body to be a diff instance for each projectile (cant be sharing that)
	PxRigidDynamic* projectileBody = dataSys->gPhysics->createRigidDynamic(spawnTransform);

	projectileBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*projectileBody, projectileMass);

	//disables gravity for the projectile
	projectileBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	//FIXME
	projectileBody->setName("temp");
	dataSys->gScene->addActor(*projectileBody);

	projectileBody->setLinearVelocity(shootForce * forwardVector);

	//adding the projectile to the dict for the correct car
	dataSys->carProjectileRigidDynamicDict[shootingCar].emplace_back(projectileBody);

	//creating the projectile entity with name based on car that shot it
	Entity projectile;
	projectile.name = dataSys->GetEntityFromRigidDynamic(shootingCar)->name + "projectile" + std::to_string(dataSys->carProjectileRigidDynamicDict[shootingCar].size());
	projectile.CreateTransformFromPhysX(projectileBody->getGlobalPose());
	projectile.physType = PhysicsType::PROJECTILE;
	projectile.collisionBox = projectileBody;

	dataSys->entityList.emplace_back(projectile);

	//subtract one ammo from the count
	dataSys->GetCarInfoStructFromEntity(dataSys->GetEntityFromRigidDynamic(shootingCar))->ammoCount--;

}
