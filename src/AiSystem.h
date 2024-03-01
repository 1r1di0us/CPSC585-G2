#pragma once
#define _USE_MATH_DEFINES
#include "PhysicsSystem.h"
#include <math.h>

enum State;

class AiSystem {
public:
	State state;

	AiSystem();
	void update(EngineDriveVehicle* aiCar);

	void sit_behaviour(EngineDriveVehicle* aiCar);
	void spin_behaviour(EngineDriveVehicle* aiCar);
	void moveto_behaviour(EngineDriveVehicle* aiCar, PxVec3 goal);
};