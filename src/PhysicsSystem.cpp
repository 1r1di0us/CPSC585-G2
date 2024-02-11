#include "PhysicsSystem.h"

//custom collision callback system
class ContactReportCallback : public PxSimulationEventCallback {
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
		PX_UNUSED(pairHeader);
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);

		std::cout << "Callback system: Stop colliding with me!" << std::endl;
	}
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {}
	void onWake(physx::PxActor** actors, physx::PxU32 count) {}
	void onSleep(physx::PxActor** actors, physx::PxU32 count) {}
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {}
	void onAdvance(const physx::PxRigidBody* const* bodyBuffer,
		const physx::PxTransform* poseBuffer,
		const physx::PxU32 count) {}
};

//gets the position of a rigid dynamic actor given index in the master list
physx::PxVec3 PhysicsSystem::getPos(int i) {
	physx::PxVec3 position = rigidDynamicList[i]->getGlobalPose().p;
	return position;
}

//updates the positions and rotations of each object using the rigid body list
void PhysicsSystem::updateTransforms() {
	for (int i = 0; i < transformList.size(); i++) {
		// store positions
		transformList[i]->pos.x = rigidDynamicList[i]->getGlobalPose().p.x;
		transformList[i]->pos.y = rigidDynamicList[i]->getGlobalPose().p.y;
		transformList[i]->pos.z = rigidDynamicList[i]->getGlobalPose().p.z;

		// store rotations
		transformList[i]->rot.x = rigidDynamicList[i]->getGlobalPose().q.x;
		transformList[i]->rot.y = rigidDynamicList[i]->getGlobalPose().q.y;
		transformList[i]->rot.z = rigidDynamicList[i]->getGlobalPose().q.z;
		transformList[i]->rot.w = rigidDynamicList[i]->getGlobalPose().q.w;
	}
}

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

	//assigning the custom callback system to our scene
	ContactReportCallback* gContactReportCallback = new ContactReportCallback();
	sceneDesc.simulationEventCallback = gContactReportCallback;

	gScene = gPhysics->createScene(sceneDesc);
	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxInitVehicleExtension(*gFoundation);
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
	gVehicleSimulationContext.setToDefault();
	gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
	gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
	gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
	gVehicleSimulationContext.scale.scale = 1.0f;
	gVehicleSimulationContext.gravity = gGravity;
	gVehicleSimulationContext.physxScene = gScene;
	gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

//does all the logic for doing one step through every vehicle movement component
void PhysicsSystem::stepAllVehicleMovementPhysics(std::vector<Car*> carList) {

	//goes through each vehicles movement component and updates them one at a time
	for (Car* car : carList) {

		EngineDriveVehicle gVehicle = car->gVehicle;

		//Forward integrate the vehicle by a single TIMESTEP.
		//Apply substepping at low forward speed to improve simulation fidelity.
		const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
		const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
		const PxReal forwardSpeed = linVel.dot(forwardDir);
		const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
		gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
		gVehicle.step(TIMESTEP, gVehicleSimulationContext);

		car->setCarTransform();

	}

}

//simulates one step of physics for all objects in scene
void PhysicsSystem::stepPhysics(std::vector<Entity> entityList) {

	//does one step for each car
	stepAllVehicleMovementPhysics(carList);

	//Forward integrate the phsyx scene by a single TIMESTEP.
	gScene->simulate(TIMESTEP);
	gScene->fetchResults(true);

	//update the transforms of each object
	this->updateTransforms();
	
}

PhysicsSystem::PhysicsSystem() { // Constructor

	//physx setup
	initPhysX();
	initGroundPlane();
	initMaterialFrictionTable();
	initVehicleSimContext();

}

//TODO: ask matt if needed
void PhysicsSystem::cleanPhysicsSystem() {

	gPhysics->release();
}

PxPhysics* PhysicsSystem::getPhysics()
{
	return gPhysics;
}

PxScene* PhysicsSystem::getScene()
{
	return gScene;
}

PxVec3 PhysicsSystem::getGravity()
{
	return gGravity;
}

PxMaterial* PhysicsSystem::getMaterial()
{
	return gMaterial;
}
