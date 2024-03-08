#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <string>

#include "Mesh.h"
#include "Shader.h"
#include "tiny_obj_loader.h"

inline unsigned int TextureFromFile(const char* path, bool gamma = false);

class Model {
public:
    Model(char* path)
    {
        loadModel(path);
    }
    void Draw(Shader& shader);

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path, std::string mtlpath);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(std::string path, std::string type);
}
#endif // !MODEL_H