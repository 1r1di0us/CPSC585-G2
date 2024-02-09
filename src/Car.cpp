#include "Car.h"

//constructor that intializes a vehicle and spawns it at given coords and rotation
Car::Car(const char* name, PxVec3 spawnPosition, PxQuat spawnRotation, PxPhysics* gPhysics, PxScene* gScene, PxVec3 gGravity, PxMaterial* gMaterial) {

	this->gPhysics = gPhysics;
	this->gScene = gScene;
	this->gMaterial = gMaterial;

	//Load the params from json or set directly.
	readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle.mBaseParams);
	setPhysXIntegrationParams(gVehicle.mBaseParams.axleDescription,
		gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
		gVehicle.mPhysXParams);
	readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json",
		gVehicle.mEngineDriveParams);

	//Set the states to default.
	if (!gVehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
	{
		printf("Car initialization failed\n");
		exit(69);
	}

	//Apply a start pose to the physx actor and add it to the physx scene.
	carTransform = PxTransform(spawnPosition, spawnRotation);
	gVehicle.setUpActor(*gScene, carTransform, name);

	PxFilterData vehicleFilter(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	//i no longer have access to this list in here
	//rigidDynamicList.push_back(gVehicle.mPhysXState.physxActor.rigidBody);

	//TODO: look at shapes and adding to rigid dynamic list

	//making all parts of the vehicle have collisions with the outside world
	PxU32 shapes = gVehicle.mPhysXState.physxActor.rigidBody->getNbShapes();
	for (PxU32 i = 0; i < shapes; i++) {
		PxShape* shape = NULL;
		gVehicle.mPhysXState.physxActor.rigidBody->getShapes(&shape, 1, i);

		//the body of the vehicle is at i = 0
		if (i == 0) {
			PxGeometryHolder carChassis = shape->getGeometry();
			vehicleBodyDimensions = carChassis.box().halfExtents * 2;
		}

		shape->setSimulationFilterData(vehicleFilter);

		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}

	//Set the vehicle in 1st gear.
	gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
	gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;

	//Set the vehicle to use the automatic gearbox.
	gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

}

//releases all the physics objects associated with the car
void Car::DestroyCar() {

}

//updates car transform
void Car::setCarTransform() {

	carTransform = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
}

//function that takes in IO and gives the appropriate command to the car
void Car::MoveCar() {



}

//shoots a projectile forward
void Car::shootProjectile() {

	// Convert quaternion to a 3x3 rotation matrix
	PxMat33 rotationMatrix(carTransform.q);

	// Rotate the forward vector using the rotation matrix (z-axis is OG forward)
	PxVec3 forwardVector = rotationMatrix.transform(PxVec3(0.0f, 0.0f, 1.0f));

	//creating the projectile to shoot
	//it is offset based on the radius of the projectile
	//TODO: THIS WILL BE REWORKED WHEN SPAWNING PROJECTILE BASED ON CAMERA DIRECTION AND TURRET SIZE
	PxTransform spawnTransform = PxTransform(
		PxVec3(carTransform.p.x + forwardVector.x * 5,
				carTransform.p.y + projectileRadius + 0.1f,
				carTransform.p.z + forwardVector.z * 5 + projectileRadius),
			carTransform.q);

	//define a projectile
	physx::PxShape* shape = gPhysics->createShape(physx::PxSphereGeometry(projectileRadius), *gMaterial);

	//creating collision flags for each projectile
	physx::PxFilterData projectileFilter(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0);
	shape->setSimulationFilterData(projectileFilter);

	//creates the rigid dynamic body to be a diff instance for each projectile (cant be sharing that)
	PxRigidDynamic* projectileBody = gPhysics->createRigidDynamic(spawnTransform);

	projectileBodyList.emplace_back(projectileBody);

	projectileBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*projectileBody, projectileMass);

	//disables gravity for the projectile
	projectileBody->setActorFlag(PxActorFlag::Enum::eDISABLE_GRAVITY, true);

	gScene->addActor(*projectileBody);

	projectileBody->setLinearVelocity(shootForce * forwardVector);
}