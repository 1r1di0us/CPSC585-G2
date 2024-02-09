#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

#include "Shader.h"
#include "Texture.h"
#include "Text.h"
#include "stb_image.h"

#include "Camera.h"

Camera camera;

class RenderingSystem {

public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;

	// variables
	GLFWwindow* window;
	unsigned int VAO, VBO, textVAO, textVBO;
	Shader textShader;
	Shader shader;
	//Camera camera;
	std::map<char, Character> Characters_gaegu;

	RenderingSystem();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer();	
	GLFWwindow* getWindow() const;

};

