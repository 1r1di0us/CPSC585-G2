#pragma once
#define _USE_MATH_DEFINES
#include "PhysicsSystem.h"
#include <math.h>
#include <iostream>
#include <chrono>
#include <random>

enum State;

class AiSystem {
public:
	SharedDataSystem* dataSys;
	State state;
	double timer;
	double brakeTimer;
	std::default_random_engine rand;
	std::normal_distribution<double> distribution;

	AiSystem(SharedDataSystem* dataSys);
	bool update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime);

	bool sit_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool spin_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool moveto_behaviour(EngineDriveVehicle* aiCar, PxVec3 goal, bool fire);
};