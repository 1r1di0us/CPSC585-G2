#pragma once

#include<glm/glm.hpp>
#include<glm/gtx/quaternion.hpp>

class Transform {
public:
	glm::vec3 pos;
	glm::quat rot;

	glm::vec3 getPos() {
		return pos;
	}

	glm::quat getRot() {
		return rot;
	}
};