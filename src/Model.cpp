#include "Model.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

// using tinyobjloader to load the model, need path for obj and mtl
void Model::loadModel(std::string path, std::string mtlpath)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path.c_str(), mtlpath.c_str());

    if (!warn.empty()) {
        std::cerr << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    if (!success) {
        std::cerr << "Failed to load/parse .obj file: " << path << std::endl;
    }

    // check if there are any shapes in the object
    if (shapes.empty())
    {
        std::cerr << "There are no shapes in this object." << std::endl;
    }
    
    if (materials.empty())
    {
        std::cerr << "There are no materials." << std::endl;
    }
    //std::cout << "Number of vertices: " << attributes.vertices.size() / 3 << std::endl;
    //std::cout << "Number of normals: " << attributes.normals.size() / 3 << std::endl;
    //std::cout << "Number of texture coordinates: " << attributes.texcoords.size() / 2 << std::endl;
    //std::cout << "Number of shapes: " << shapes.size() << std::endl;
    //std::cout << "Number of materials: " << materials.size() << std::endl;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // for all shape in shapes vector
    for (const auto& shape : shapes) {

        // for each each mesh, get the vertices, indices, and textures
        //std::cout << "Number of mesh: " << shape.mesh.indices.size() << std::endl;
        //shape.mesh.indices[x] is accessing information at vertex x
        for (const auto& index : shape.mesh.indices) {

            Vertex vertex;
            // create the single vertex data
            vertex.Position = glm::vec3(
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2]
            );

            if (!attributes.normals.empty()) {
                vertex.Normal = glm::vec3(
                    attributes.normals[3 * index.normal_index + 0],
                    attributes.normals[3 * index.normal_index + 1],
                    attributes.normals[3 * index.normal_index + 2]
                );

            }

            if (!attributes.texcoords.empty()) {
                vertex.TexCoords = glm::vec2(
                    attributes.texcoords[2 * index.texcoord_index + 0],
                    attributes.texcoords[2 * index.texcoord_index + 1]
                );
            }

            vertices.push_back(vertex);
            // I'm hoping that the below is the correct thing, below is to get the indices for index rendering
            indices.push_back(index.vertex_index);

        }
        // get the textures for each mesh
        // store diffuse and specular only for now
        for (size_t i = 0; i < shape.mesh.material_ids.size(); ++i) {
            int material_id = shape.mesh.material_ids[i];
            // check if there is actually a material here
            if (material_id >= 0 && material_id < materials.size()) {
                tinyobj::material_t& material = materials[material_id];
                
                // Check if it has a diffuse texture, if so load it
                if (!material.diffuse_texname.empty()) {

                    std::vector<Texture> diffuseMaps = loadMaterialTextures(path, material.diffuse_texname);
                    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
                }

                // Check if it has a specular texture, if so load it
                if (!material.specular_texname.empty()) {
                    std::vector<Texture> specularMaps = loadMaterialTextures(path, material.specular_texname);
                    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

                }
            }
        }
    }
}

// loads all the material textures associated with that mesh
std::vector<Texture> Model::loadMaterialTextures(std::string path, std::string type, )
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < length; i++)
    {

    }
    Texture texture;
    texture.id = TextureFromFile(path, true);
    texture.type = type;
    texture.path = path;
    textures.push_back(texture);
    
    
    return std::vector<Texture>();
}

unsigned int TextureFromFile(const std::string& path, bool gamma)
{
    std::string filename = std::string(path);
    filename = path + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}