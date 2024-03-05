#define TINYOBJLOADER_IMPLEMENTATION
#include "ModelLoader.h"

using glm::vec2;
using glm::vec3;
using glm::uvec3;
using std::vector;

//Mesh LoadModelFromPathMesh(std::string modelFilePath) {
//    tinyobj::attrib_t attributes;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string warn;
//    std::string err;
//    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, modelFilePath.c_str(), "./assets/Models/");
//
//    if (!warn.empty()) {
//        std::cerr << "Warning: " << warn << std::endl;
//    }
//
//    if (!err.empty()) {
//        std::cerr << "Error: " << err << std::endl;
//    }
//
//    if (!success) {
//        std::cerr << "Failed to load/parse .obj file: " << modelFilePath << std::endl;
//    }
//    
//    std::vector<Vertex> vertices;
//    std::vector<Texture> textures;
//    std::vector<unsigned int> indices;
//
//
//    for (const auto& shape : shapes) {
//        for (const auto& index : shape.mesh.indices) {
//            //stores the vertex index
//            indices.push_back(index.vertex_index);
//
//            //start storing the vertex data
//            glm::vec2 texCoord;
//            glm::vec3 position, normal;
//
//            //storing the verticies
//            position.x = attributes.vertices[3 * index.vertex_index + 0];
//            position.y = attributes.vertices[3 * index.vertex_index + 1];
//            position.z = attributes.vertices[3 * index.vertex_index + 2];
//
//            //storing the normals
//            if (!attributes.normals.empty()) {
//                normal.x = attributes.normals[3 * index.normal_index + 0];
//                normal.y = attributes.normals[3 * index.normal_index + 1];
//                normal.z = attributes.normals[3 * index.normal_index + 2];
//            }
//            
//             storing the texcoords
//            if (!attributes.texcoords.empty()) {
//                texCoord.x = attributes.texcoords[2 * index.texcoord_index + 0];
//                texCoord.y = attributes.texcoords[2 * index.texcoord_index + 1];
//            }
//
//            Vertex vertex(position, normal, texCoord);
//            vertices.push_back(vertex);
//
//            //get texture information
//            for (int i = 0; i < materials.size(); i++)
//            {
//                unsigned int id = materials[i].
//            }
//
//
//             get indices information
//            for (size_t s = 0; s < shapes.size(); s++)
//            {
//                for (size_t f = 0; f < shapes[s].mesh.indices.size(); f++) {
//                    indices.push_back(shapes[s].mesh.indices[f].vertex_index);
//                     Do something with vertex_index
//                    std::cout << "Vertex index: " << shapes[s].mesh.indices[f].vertex_index << std::endl;
//                }
//            }
//
//            
//        }
//    }
//
//    Mesh modelMesh(vertices, indices, textures);
//
//    return modelMesh;
//}

OBJModel LoadModelFromPath(std::string modelFilePath){
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, modelFilePath.c_str(), "./assets/Models/");

    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load/parse .obj file: " << modelFilePath << std::endl;
    }

    //std::cout << "Number of vertices: " << attributes.vertices.size() / 3 << std::endl;
    //std::cout << "Number of normals: " << attributes.normals.size() / 3 << std::endl;
    //std::cout << "Number of texture coordinates: " << attributes.texcoords.size() / 2 << std::endl;
    //std::cout << "Number of shapes: " << shapes.size() << std::endl;
    //std::cout << "Number of materials: " << materials.size() << std::endl;


    OBJModel model;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            
            glm::vec3 vertices(
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2]
            );
            model.vertices.push_back(vertices);

            if (!attributes.texcoords.empty()) {
                glm::vec2 texCoord(
                    attributes.texcoords[2 * index.texcoord_index + 0],
                    attributes.texcoords[2 * index.texcoord_index + 1]
                );
                model.textureCoordinates.push_back(texCoord);
            }

            if (!attributes.normals.empty()) {
                glm::vec3 normal(
                    attributes.normals[3 * index.normal_index + 0],
                    attributes.normals[3 * index.normal_index + 1],
                    attributes.normals[3 * index.normal_index + 2]
                );
                model.normals.push_back(normal);
            }

            //model.indices.push_back(model.indices.size());
            model.indices.push_back(model.vertices.size() - 1);

        }
    }

    return model;
}
