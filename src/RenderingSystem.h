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


#include <glm/glm.hpp>
#include <vector>
#include "Camera.h"

#include <glm/gtc/random.hpp>

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <chrono>

#include "Shader.h"
#include "Texture.h"

class RenderingSystem {

private:
	SharedDataSystem* dataSys;

public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;

	// variables
	GLFWwindow* window;
	unsigned int VAO, VBO,
		textVAO, textVBO,
		tankVAO, tankVBO,
		ballVAO, ballVBO,
		buildingVAO, buildingVBO,
		planeVAO, planeVBO,
		powerupVAO, powerupVBO,
		quadVAO, quadVBO,
		particlesVAO, particlesVBO;
	Shader textShader;
	Shader shader;
	Shader ourShader; // testing it
	std::map<char, Character> Characters_gaegu;
	OBJModel tank, building, ball, plane, powerup, bedModel, particleObj;

	RenderingSystem(SharedDataSystem* dataSys);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void updateRenderer(Camera camera, std::chrono::duration<double> timeLeft, std::chrono::duration<double> deltaTime);
	//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	GLFWwindow* getWindow() const;

	// Generate particles at a given position
	void generateParticles(glm::vec3 position, int count);

	// Update particle positions and properties
	void particleUpdate(float deltaTime);

	// Render particles
	void particleRender(std::chrono::duration<double> deltaTime, glm::mat4 model);

	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 color;
		float size;
		float lifetime;
	};

	std::vector<Particle> particles;
	unsigned int particleVAO, particleVBO;
	unsigned int particleTexture; // Texture for particles
	Shader particleShader;
	void initParticlesVAO();
};

//void drawFromModel(std::string modelFilePath);
void renderOBJ(const OBJModel& model);
void renderObject(const OBJModel& model, unsigned int* VAO);
void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO);