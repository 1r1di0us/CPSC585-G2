#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class RenderingSystem {

public:
	// variables
	GLFWwindow* window;
	unsigned int VBO, VAO;
	Shader textShader;
	Shader shader;
	//Camera camera;

	RenderingSystem();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer();	
	GLFWwindow* getWindow() const;
};

