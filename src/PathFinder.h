#pragma once
#include "PhysicsSystem.h"
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <iostream>

class Node { //no idea why this class is defined here but ok
public: // A "Node" is a square
	unsigned int id;
	PxVec3 v0; //top left
	PxVec3 v1; //top right
	PxVec3 v2; //bottom right
	PxVec3 v3; //bottom left
	PxVec3 centroid;


	std::vector<std::pair<float, Node*>>* connections;

	PxVec3 getCentroid(Node* node) {
		return (node->v0 + node->v1 + node->v2 + node->v3) / 4.f;
	}

	Node(unsigned int id, PxVec3 v0, PxVec3 v1, PxVec3 v2, PxVec3 v3) {
		this->id = id;
		this->v0 = v0;
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->centroid = getCentroid(this);
		this->connections = new std::vector<std::pair<float, Node*>>(); //initialize connections manually
	}
};

class NavMesh {
public:
	std::map<unsigned int, Node*>* nodes;

	NavMesh(SharedDataSystem* dataSys); //constructor

	Node* findEntity(PxVec3 pos);

private:
	SharedDataSystem* dataSys;
	float cost(Node* src, Node* dest);
};

class PathFinder {

public:

	std::stack<PxVec3> path;
	NavMesh* navMesh;

	PathFinder(NavMesh* navMesh);

	bool search(Node* src, Node* dest);
	PxVec3 getNextWaypoint();

private:
	bool isDestination(Node* src, Node* dest);
	float calculateHCost(Node* src, Node* dest);
	void tracePath(Node* src, Node* dest, std::map<unsigned int, unsigned int> parents);

};