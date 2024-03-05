#include "PhysicsSystem.h"

//initializes physx
void PhysicsSystem::initPhysX() {

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = gGravity;

	PxU32 numWorkers = 1;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;

	gScene = gPhysics->createScene(sceneDesc);

	//assigning the custom callback system to our scene
	gScene->setSimulationEventCallback(dataSys->gContactReportCallback);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(1.0f, 1.0f, 0.0f);

	PxInitVehicleExtension(*gFoundation);
	
	//setting all the datasys variables
	dataSys->gPhysics = gPhysics;
	dataSys->gScene = gScene;
	dataSys->gMaterial = gMaterial;
	
}

//creates the ground
void PhysicsSystem::initGroundPlane() {

	gGroundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	for (PxU32 i = 0; i < gGroundPlane->getNbShapes(); i++)
	{
		PxShape* shape = NULL;
		gGroundPlane->getShapes(&shape, 1, i);
		shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}
	gScene->addActor(*gGroundPlane);
}

void PhysicsSystem::initMaterialFrictionTable() {

	//Each physx material can be mapped to a tire friction value on a per tire basis.
	//If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
	//In this snippet there is only a single material so there can only be a single mapping between material and friction.
	//In this snippet the same mapping is used by all tires.
	gPhysXMaterialFrictions[0].friction = 5.0f;
	gPhysXMaterialFrictions[0].material = gMaterial;
	gPhysXDefaultMaterialFriction = 5.0f;
	gNbPhysXMaterialFrictions = 5;
}

void PhysicsSystem::initVehicleSimContext() {
	//Set up the simulation context.
	//The snippet is set up with
	//a) z as the longitudinal axis
	//b) x as the lateral axis
	//c) y as the vertical axis.
	//d) metres  as the lengthscale.
	this->gVehicleSimulationContext.setToDefault();
	this->gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	this->gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	this->gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
	this->gVehicleSimulationContext.scale.scale = 1.0f;
	this->gVehicleSimulationContext.gravity = gGravity;
	this->gVehicleSimulationContext.physxScene = gScene;
	this->gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

//does all the logic for doing one step through every vehicle movement component
void PhysicsSystem::stepAllVehicleMovementPhysics() {

	//goes through each vehicles movement component and updates them one at a time
	for (int i = 0; i < dataSys->gVehicleList.size(); i++) {

		//simulate only if the vehicle is alive
		if (dataSys->carInfoList[i].isAlive) {

			//Forward integrate the vehicle by a single TIMESTEP.
			//Apply substepping at low forward speed to improve simulation fidelity.
			const PxVec3 linVel = dataSys->carRigidDynamicList[i]->getLinearVelocity();
			const PxVec3 forwardDir = dataSys->carRigidDynamicList[i]->getGlobalPose().q.getBasisVector2();
			const PxReal forwardSpeed = linVel.dot(forwardDir);
			const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);

			dataSys->gVehicleList[i]->mComponentSequence.setSubsteps(dataSys->gVehicleList[i]->mComponentSequenceSubstepGroupHandle, nbSubsteps);
			dataSys->gVehicleList[i]->step(dataSys->TIMESTEP, this->gVehicleSimulationContext);
		}
	}

}

//simulates one step of physics for all objects in scene
void PhysicsSystem::stepPhysics() {

	//does one step for each car
	stepAllVehicleMovementPhysics();

	//Forward integrate the phsyx scene by a single TIMESTEP.
	gScene->simulate(dataSys->TIMESTEP);
	gScene->fetchResults(true);

	dataSys->ResolveCollisions();

	//update the transform components of each entity
	for (Entity entity : dataSys->entityList) {
		entity.updateTransform();
	}
	
}

PhysicsSystem::PhysicsSystem(SharedDataSystem* dataSys) { // Constructor

	this->dataSys = dataSys;

	//physx setup
	initPhysX();
	initGroundPlane();
	initMaterialFrictionTable();
	initVehicleSimContext();

}

void PhysicsSystem::releaseActors() {
	/*
	* RIGID DYNAMIC ACTORS
	*/

	auto type = PxActorTypeFlag::eRIGID_DYNAMIC;
	// Get the number of actors in the scene
	PxU32 numActors = gScene->getNbActors(type);
	// Allocate memory to store the actors
	PxActor** userBuffer = new physx::PxActor * [numActors];
	// Get all actors in the scene
	gScene->getActors(type, userBuffer, numActors);

	//delete all rigid dynamic actors
	for (int i = 0; i < numActors; i++) {

		gScene->removeActor(*userBuffer[i]);
		userBuffer[i]->release();
	}
}

//TODO: make this good
void PhysicsSystem::cleanPhysicsSystem() {
	//gScene->release();
	gPhysics->release();
	//gDispatcher->release();
	//gMaterial->release();
}
