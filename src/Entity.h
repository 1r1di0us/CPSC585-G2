#pragma once

#include <string>
#include "Transform.h"
#include "Model.h"
#include "snippetvehicle2common/enginedrivetrain/EngineDrivetrain.h"
#include "snippetvehicle2common/serialization/BaseSerialization.h"
#include "snippetvehicle2common/serialization/EngineDrivetrainSerialization.h"
#include "snippetvehicle2common/SnippetVehicleHelpers.h"
#include "snippetcommon/SnippetPVD.h"

using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

enum class PhysicsType {
	CAR,
	PROJECTILE,
	STATIC
};
	
class Entity {

public:
	std::string name;

	Model* model;
	Transform* transform;

	PhysicsType physType;
	PxRigidDynamic* collisionBox;

	//helper functions
	void updateTransform();

	//utility function to create an entities transform component given a physx transform
	void CreateTransformFromPhysX(PxTransform physXTransform);
};