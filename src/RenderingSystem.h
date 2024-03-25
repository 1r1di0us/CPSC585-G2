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
	Model plane, projectile, tank, powerup, tankHead, tankBody, tankWheels, tankWheelFR, tankWheelFL, tankWheelBL, tankWheelBR, bunny, train, toyBlock;

	// galaxy skybox
	std::vector<std::string> faces
	{
		"./assets/skybox/px.jpg",
		"./assets/skybox/nx.jpg",
		"./assets/skybox/py.jpg",
		"./assets/skybox/ny.jpg",
		"./assets/skybox/pz.jpg",
		"./assets/skybox/nz.jpg"
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

unsigned int loadCubemap(std::vector<std::string> faces);
void renderDeathScreen(unsigned int* VAO);

