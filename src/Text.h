#pragma once

#include <glm.hpp>
#include <map>
#include "Shader.h"

struct Character {
	unsigned int textID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int Advance;
};

std::map<char, Character> initFont(const char* font);
void RenderText(Shader& s, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color, std::map<char, Character> Characters);
