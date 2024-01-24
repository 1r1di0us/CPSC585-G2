#include <glm.hpp>
#include <map>
#include "shader_s.h"

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> initFont(const char* font);
void RenderText(Shader& s, unsigned int VAO, unsigned int VBO, std::string text, float x, float y, float scale, glm::vec3 color, std::map<char, Character> Characters);