#define TINYOBJLOADER_IMPLEMENTATION
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ModelLoader.h"

using glm::vec2;
using glm::vec3;
using glm::uvec3;
using std::vector;

int ModelLoaderUpdate(){

    //objl::Loader loader;

    //loader.LoadFile("src/assets/models/bed_double_A.obj");

    //std::cout << loader.LoadedMeshes[0].MeshName << std::endl;

    std::string inputfile = "../assets/Models/plane.obj";

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), "../assets/Models/plane.mtl");

    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load/parse .obj file: " << inputfile << std::endl;
        return 1;
    }

    std::cout << "Number of vertices: " << attrib.vertices.size() / 3 << std::endl;
    std::cout << "Number of normals: " << attrib.normals.size() / 3 << std::endl;
    std::cout << "Number of texture coordinates: " << attrib.texcoords.size() / 2 << std::endl;
    std::cout << "Number of shapes: " << shapes.size() << std::endl;
    std::cout << "Number of materials: " << materials.size() << std::endl;

    return 0;
}