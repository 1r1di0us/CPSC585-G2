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

	//make all the connections
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

PathFinder::PathFinder(NavMesh* navMesh) {
	this->navMesh = navMesh;
}

bool PathFinder::search(Node* src, Node* dest) {

	if (src == NULL) {
		std::cout << "NULL SOURCE" << std::endl;
		return false;
	}
	else if (dest == NULL) {
		std::cout << "NULL DESTINATION" << std::endl;
		return false;
	}

	if (isDestination(src, dest)) {
		std::cout << "ALREADY AT DESTINATION!" << std::endl;
		return true;
	}

	// Search data structs

	std::map<unsigned int, bool> explored;
	std::set<std::pair<float, Node*>> frontier;
	std::map<unsigned int, unsigned int> parents;

	// add src node to frontier (0 cost)

	frontier.insert(std::make_pair(0.f, src));

	// And to explored
	explored.insert({ frontier.begin()->second->id, false });

	// And to parent
	parents.insert({ src->id, src->id });

	// Loop !

	while (!frontier.empty()) {

		// Get a reference to first in frontier, and remove it

		std::pair<float, Node*> p = *frontier.begin();
		frontier.erase(frontier.begin());

		// Add to explored
		explored.find(p.second->id)->second = true;

		float gNew = 0;
		float hNew = 0;
		float fNew = 0;

		// Init connections in explored
		for (unsigned int i = 0; i < p.second->connections->size(); i++) {

			if (explored.find(p.second->connections->at(i).second->id) == explored.end()) {
				explored.insert({ p.second->connections->at(i).second->id, false });
			}
		}

		// Go through all connections
		for (unsigned int i = 0; i < p.second->connections->size(); i++) {

			//get reference
			std::pair<float, Node*> temp = p.second->connections->at(i);


			//Check if we at destination
			if (isDestination(temp.second, dest)) {
				parents.insert_or_assign(temp.second->id, p.second->id);
				this->tracePath(temp.second, dest, parents);

				return true;
			}

			// Have not explored this node yet
			else if (!explored.find(temp.second->id)->second) {
				// Update cost!

				// Cost so far // Cost of current edge
				gNew = p.first + temp.first; // Actual cost
				hNew = calculateHCost(temp.second, dest); // Heuristic cost
				fNew = gNew + hNew; // Total Cost

				// Add to frontier
				frontier.insert(std::make_pair(fNew, temp.second));
				parents.insert_or_assign(temp.second->id, p.second->id);
			}
		}

		// If we loop through and never find the destination
		std::cout << "THE DESTINATION CELL IS NOT FOUND" << std::endl;
		return false;

	}
}

bool PathFinder::isDestination(Node * src, Node * dest) {
	if (src->id == dest->id) {
		return true;
	}
	else {
		return false;
	}
}

glm::vec3 PathFinder::getNextWaypoint() {
	if (path->size() > 0) {
		glm::vec3 vector = path->top();
		path->pop();

		return vector;
	}
	else {
		std::cout << "Path is Empty :(" << std::endl;
	}

}

float PathFinder::calculateHCost(Node* src, Node* dest) {
	glm::vec3 srcCenter = src->v0 + src->v1 + src->v2 / 3.f;
	glm::vec3 destCenter = dest->v0 + dest->v1 + dest->v2 / 3.f;

	// get euc dist
	float dx = glm::abs(srcCenter.x - destCenter.x);
	float dy = glm::abs(srcCenter.y - destCenter.y);
	float dz = glm::abs(srcCenter.z - destCenter.z);

	return glm::sqrt(dx * dx + dy * dy + dz * dz);
}

void PathFinder::tracePath(Node* src, Node* dest, std::map<unsigned int, unsigned int> parents) {
	std::vector<glm::vec3> bPath;

	// Get rid of any pre-existing paths
	while (!this->path->empty()) {
		this->path->pop();
	}

	// traverse
	unsigned int temp = dest->id;
	while (temp != parents.find(temp)->second) {
		bPath.push_back(this->navMesh->nodes->find(temp)->second->centroid);
		path->push(this->navMesh->nodes->find(temp)->second->centroid);
		temp = parents.find(temp)->second;
	}
	bPath.push_back(this->navMesh->nodes->find(temp)->second->centroid);
	path->push(this->navMesh->nodes->find(temp)->second->centroid);
}