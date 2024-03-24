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

enum class STATE { hiding, hunting };

class AiSystem {
public:
	SharedDataSystem* dataSys;
	STATE state;
	NavMesh* navMesh;
	PathFinder* pathFinder;
	double startTimer;
	double brakeTimer; //for shooting
	double coolDownTimer;
	double reverseTimer;
	double lockOnTime;
	PxVec3 moveLocation;
	Node* moveNode;
	int nodeIterator = 0;
	float enemyDist = 0;
	std::vector<Node*> centerNodes; //for patrolling
	std::vector<Node*> edgeNodes;
	EngineDriveVehicle* aiCar;
	CarInfo* target = nullptr;
	CarInfo* aiCarInfo = nullptr;
	physx::PxRigidDynamic* targetPowerup = nullptr;
	PxVec3 aimDir = PxVec3(0, 0, -1);
	bool wantToFire = false;
	bool lockedOn = false;
	bool movingToPowerup = false;
	bool transitioning = false; //lets us know when we are transitioning from one state to another

	PxVec3 carPos;

	AiSystem();
	AiSystem(SharedDataSystem* dataSys, EngineDriveVehicle* aiCar);
	bool update(std::chrono::duration<double> deltaTime);
	bool hiding_behaviour(bool fire);
	bool hunting_behaviour(bool fire);
	void astar_path_finding();
	void move_car();
	void aim_car(std::chrono::duration<double> deltaTime);
};