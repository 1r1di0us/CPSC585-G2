#pragma once
#define _USE_MATH_DEFINES
#include "PhysicsSystem.h"
#include <math.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>
#include "PathFinder.h"

class AiSystem {
public:
	SharedDataSystem* dataSys;
	NavMesh* navMesh;
	PathFinder* pathFinder;
	double startTimer;
	double brakeTimer; //for shooting
	double coolDownTimer;
	double lockOnTime;
	PxVec3 moveLocation;
	Node* moveNode;
	int nodeIterator = 0;
	std::vector<Node*> centerNodes; //for patrolling
	std::vector<Node*> edgeNodes;
	CarInfo* target = nullptr;
	CarInfo* aiCarInfo = nullptr;
	bool wantToFire = false;
	bool lockedOn = false;

	AiSystem(SharedDataSystem* dataSys);
	bool update(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime);
	bool hiding_behaviour(EngineDriveVehicle* aiCar, bool fire);
	bool hunting_behaviour(EngineDriveVehicle* aiCar, bool fire);
	void astar_path_finding(EngineDriveVehicle* aiCar);
	void move_car(EngineDriveVehicle* aiCar);
	void aim_car(EngineDriveVehicle* aiCar, std::chrono::duration<double> deltaTime);

};