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

class RenderingSystem {

public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;

	// variables
	GLFWwindow* window;
	unsigned int VAO, VBO, textVAO, textVBO, tankVAO, tankVBO, ballVAO, ballVBO, buildingVAO, buildingVBO, planeVAO, planeVBO;
	Shader textShader;
	Shader shader;
	std::map<char, Character> Characters_gaegu;
	OBJModel tank, building, ball, plane;

	RenderingSystem();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void updateRenderer(std::vector<Entity> entityList, Camera camera, std::chrono::duration<double> timeLeft, Entity *playerCar);
	//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	GLFWwindow* getWindow() const;

};

//void drawFromModel(std::string modelFilePath);
void renderOBJ(const OBJModel& model);
void renderObject(const OBJModel& model, unsigned int* VAO);
void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO);