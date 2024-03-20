#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <chrono>

#include "Shader.h"
#include "Texture.h"
#include "Text.h"
#include "stb_image.h"
#include "ModelLoader.h"
#include "vector"
#include "Entity.h"
#include "Camera.h"
#include "SharedDataSystem.h"
#include "Model.h"
#include "Mesh.h"

class RenderingSystem {

private:
	SharedDataSystem* dataSys;
	Model bedModel, funkyCube, plane, projectile, tank, powerup;
	std::vector<std::string> faces
	{
		"./assets/skybox/right.jpg",
		"./assets/skybox/left.jpg",
		"./assets/skybox/top.jpg",
		"./assets/skybox/bottom.jpg",
		"./assets/skybox/front.jpg",
		"./assets/skybox/back.jpg"
	};
	
    



public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// variables
	GLFWwindow* window;
	unsigned int VAO, VBO, textVAO, textVBO, skyVAO, skyVBO;
	Shader textShader;
	Shader shader;
	Shader skyBoxShader;
	Shader testShader; // testing it
	std::map<char, Character> Characters_gaegu;
	unsigned int cubemapTexture;

	RenderingSystem(SharedDataSystem* dataSys);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer(Camera camera, std::chrono::duration<double> timeLeft);
	//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	GLFWwindow* getWindow() const;

};

void renderObject(const OBJModel& model, unsigned int* VAO);
void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO);
unsigned int loadCubemap(std::vector<std::string> faces);

