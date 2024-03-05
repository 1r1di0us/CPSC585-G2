#include "tiny_obj_loader.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Mesh.h"

struct OBJModel {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
};

class ModelLoader {
public:

	
};


OBJModel LoadModelFromPath(std::string modelFilePath);