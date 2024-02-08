#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <tinyobjloader/tiny_obj_loader.h>
#include <stdexcept>

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Model {
public:
	std::vector<Vertex> verts;
	glm::mat4 modelMatrix;

	void loadModel(const std::string& filepath);
};

