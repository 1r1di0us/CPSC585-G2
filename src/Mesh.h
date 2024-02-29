#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    Vertex(glm::vec3 positions, glm::vec3 normals, glm::vec2 texCoords) {
        Position = positions;
        Normal = normals;
        TexCoords = texCoords;
    }
};

struct Texture {
    unsigned int id;
    std::string type;
};

class Mesh {
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    Mesh() {
    };

    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};