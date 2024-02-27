#include "CarSystem.h"

CarSystem::CarSystem(PxPhysics* gPhysics, PxScene* gScene, PxMaterial* gMaterial, std::vector<Entity>* entityList) {

	this->gPhysics = gPhysics;
	this->gScene = gScene;
	this->gMaterial = gMaterial;
	this->entityList = entityList;

	//assigning the custom callback system to our scene
	gScene->setSimulationEventCallback(this->gContactReportCallback);

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
	if (!gVehicle->initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
	{
		printf("Car initialization failed\n");
		exit(69);
	}

	//Apply a start pose to the physx actor and add it to the physx scene.
	PxTransform carTransform = PxTransform(spawnPosition, spawnRotation);
	gVehicle->setUpActor(*gScene, carTransform, name);

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

		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	//Set the vehicle in 1st gear.
	gVehicle->mEngineDriveState.gearboxState.currentGear = gVehicle->mEngineDriveParams.gearBoxParams.neutralGear + 1;
	gVehicle->mEngineDriveState.gearboxState.targetGear = gVehicle->mEngineDriveParams.gearBoxParams.neutralGear + 1;

	//Set the vehicle to use the automatic gearbox.
	gVehicle->mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

	//adding car to needed lists
	carRigidDynamicList.emplace_back((PxRigidDynamic*)gVehicle->mPhysXState.physxActor.rigidBody);
	gVehicleList.emplace_back(gVehicle);
	projectileRigidDynamicDict[gVehicle] = std::vector<PxRigidDynamic*>();

	//creating the car entity to add to the entity list
	Entity car;
	car.name = "car" + std::to_string(gVehicleList.size());
	car.CreateTransformFromPhysX(gVehicle->mPhysXState.physxActor.rigidBody->getGlobalPose());
	car.physType = PhysicsType::CAR;
	car.collisionBox = carRigidDynamicList.back();

	entityList->emplace_back(car);
}

void CarSystem::RespawnCar(EngineDriveVehicle* carToRespawn) {


}

EngineDriveVehicle* CarSystem::GetVehicleFromRigidDynamic(PxRigidDynamic* carRigidDynamic) {

	for (int i = 0; i < carRigidDynamicList.size(); i++) {

		if (carRigidDynamicList[i] == carRigidDynamic) {
			return gVehicleList[i];
		}
	}

	//unreachable code
	exit(69);
}

Entity* CarSystem::GetEntityFromRigidDynamic(PxRigidDynamic* rigidDynamic) {

	for (int i = 0; i < entityList->size(); i++) {

		if (entityList->at(i).collisionBox == rigidDynamic) {
			return &entityList->at(i);
		}
	}
	//unreachable code
	exit(69);
}

void CarSystem::CollideCarProjectile(PxRigidDynamic* car, PxRigidDynamic* projectile) {


}

std::vector<EngineDriveVehicle*> CarSystem::GetGVehicleList() {
	return this->gVehicleList;
}

void CarSystem::Shoot(EngineDriveVehicle* shootingCar) {

	//gets the forward vector of the car
	PxVec3 forwardVector = shootingCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();

	//creating the projectile to shoot
	//it is offset based on the radius of the projectile
	//TODO: THIS WILL BE REWORKED WHEN SPAWNING PROJECTILE BASED ON CAMERA DIRECTION AND TURRET SIZE
	PxTransform spawnTransform = PxTransform(
		PxVec3(shootingCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p.x + forwardVector.x * 5,
			shootingCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p.y + projectileRadius + 0.1f,
			shootingCar->mPhysXState.physxActor.rigidBody->getGlobalPose().p.z + forwardVector.z * 5),
		shootingCar->mPhysXState.physxActor.rigidBody->getGlobalPose().q);

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxSphereGeometry(projectileRadius), *gMaterial);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_PROJECTILE, COLLISION_FLAG_PROJECTILE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	//creates the rigid dynamic body to be a diff instance for each projectile (cant be sharing that)
	PxRigidDynamic* projectileBody = gPhysics->createRigidDynamic(spawnTransform);

	projectileBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*projectileBody, projectileMass);

	//disables gravity for the projectile
	projectileBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	projectileBody->setName("temp");
	gScene->addActor(*projectileBody);

	projectileBody->setLinearVelocity(shootForce * forwardVector);

	//adding the projectile to the dict for the correct car
	projectileRigidDynamicDict[shootingCar].emplace_back(projectileBody);

	//creating the projectile entity with name based on 
	Entity projectile;
	projectile.name = GetEntityFromRigidDynamic((PxRigidDynamic*)shootingCar->mPhysXState.physxActor.rigidBody)->name + "projectile" + std::to_string(projectileRigidDynamicDict[shootingCar].size());
	projectile.CreateTransformFromPhysX(projectileBody->getGlobalPose());
	projectile.physType = PhysicsType::PROJECTILE;
	projectile.collisionBox = projectileBody;

	entityList->emplace_back(projectile);

}

void CarSystem::DestroyProjectile(PxRigidDynamic* projectileToDestroy) {

	if (this->gContactReportCallback->contactDetected) {
		if (this->gContactReportCallback->contactPair.actors[0]->getName() == "temp") {
			gScene->removeActor(*gContactReportCallback->contactPair.actors[0]);
			this->gContactReportCallback->contactPair.actors[0]->release();
		}
		else {
			gScene->removeActor(*gContactReportCallback->contactPair.actors[1]);
			this->gContactReportCallback->contactPair.actors[1]->release();
		}
	}

}


