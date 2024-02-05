#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class RenderingSystem {

public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// variables
	GLFWwindow* window;
	//unsigned int VAO, VBO;
	//Shader textShader;
	//Shader shader;
	//Camera camera;
	//std::map<char, Character> Characters;

	RenderingSystem();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer();	
	GLFWwindow* getWindow() const;
};

