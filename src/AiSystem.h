#pragma once
#define _USE_MATH_DEFINES
#include "PhysicsSystem.h"
#include <math.h>
#include <iostream>
#include <chrono>
#include <random>
#include <map>
#include "PathFinder.h"

//DELETE THIS
enum State;

class AiSystem {
public:
	SharedDataSystem* dataSys;
	NavMesh* navMesh;
	PathFinder* pathFinder;
	State state;
	double timer;
	double brakeTimer;
	PxVec3 moveLocation;
	std::default_random_engine rand;
	std::normal_distribution<double> distribution;

	AiSystem(SharedDataSystem* dataSys);
	bool update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime);
	bool hiding_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool hunting_behaviour(EngineDriveVehicle* aiCar, bool fire);
	void astar_path_finding(EngineDriveVehicle* aiCar);

	//TO BE DELETED
	bool update_old(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime, PxVec3 movLoc);
	bool sit_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool spin_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool moveto_behaviour(EngineDriveVehicle* aiCar, bool fire);
};