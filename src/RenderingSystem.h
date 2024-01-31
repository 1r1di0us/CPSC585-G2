#include <glad/glad.h>
#include <GLFW/glfw3.h>

class RenderingSystem {

private:
	GLFWwindow* window;

public:
	RenderingSystem();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void render();
	GLFWwindow* getWindow() const;
};

