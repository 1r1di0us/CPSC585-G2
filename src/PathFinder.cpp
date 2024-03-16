#include "PathFinder.h"

NavMesh::NavMesh() {
	this->nodes = new std::map<unsigned int, Node*>();

	//make vertices
	std::vector<std::vector<glm::vec3>> verts = std::vector<std::vector<glm::vec3>>();
	int i = 0;
	for (float x = -75; x <= 75; x += 5.f) { //make vertices every 5 units and add them into the vertices pile
		verts.emplace_back(std::vector<glm::vec3>());
		for (float z = -75; z <= 75; z += 5.f) { // z is horizontal yes
			verts[i].emplace_back(glm::vec3(x, 0.f, z));  //31 x 31 vertices
		}
		i++;
	}

	//make nodes and fill map with nodes
	int id = 0;
	for (int x = 0; x < 30; x++) {
		for (int z = 0; z < 30; z++) {
			this->nodes->insert({ id, new Node(id, verts[x][z], verts[x][z + 1], verts[x + 1][z + 1], verts[x + 1][z]) });
			id++;
		}
	}

	for (int x = 0; x < 30; x++) {
		for (int z = 0; z < 30; z++) {
			if (x > 0 && z > 0) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x - 1) * 30 + (z - 1))), this->nodes->at((x - 1) * 30 + (z - 1)))); //top left
			if (z > 0) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at(x * 30 + (z - 1))), this->nodes->at(x * 30 + (z - 1)))); //top centre
			if (x < 29 && z > 0) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x + 1) * 30 + (z - 1))), this->nodes->at((x + 1) * 30 + (z - 1)))); //top right
			if (x < 29) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x + 1) * 30 + z)), this->nodes->at((x + 1) * 30 + z))); //centre right
			if (x < 29 && z < 29) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x + 1) * 30 + (z + 1))), this->nodes->at((x + 1) * 30 + (z + 1)))); //bottom right
			if (z < 29) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at(x * 30 + (z + 1))), this->nodes->at(x * 30 + (z + 1)))); //bottom centre
			if (x > 0 && z < 29) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x - 1) * 30 + (z + 1))), this->nodes->at((x - 1) * 30 + (z + 1)))); //bottom left
			if (x > 0) this->nodes->at(x * 30 + z)->connections->emplace_back(std::make_pair(cost(this->nodes->at(x * 30 + z), this->nodes->at((x - 1) * 30 + z)), this->nodes->at((x - 1) * 30 + z))); //centre left
		}
	}
}

float NavMesh::cost(Node* src, Node* dest)
{
	// calc distance between centers
	float dx = glm::abs(src->centroid.x - dest->centroid.x);
	//float dy = glm::abs(src->centroid.y - dest->centroid.y);
	float dy = 0; //all the ys are 0 atm
	float dz = glm::abs(src->centroid.z - dest->centroid.z);

	return glm::sqrt(dx * dx + dy * dy + dz * dz);
}