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
	Model plane, projectile, tank,
		ammoPowerup, projectileSpeedPowerup, projectileSizePowerup, armourPowerup,
		tankHead, tankBody, tankWheel, bunny, train, toyBlock1, toyBlock2;

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
	unsigned int VAO, VBO, textVAO, textVBO, skyVAO, skyVBO, particlesVAO, particlesVBO;
	Shader textShader;
	Shader shader;
	Shader skyBoxShader;
	Shader testShader; // testing it
	std::map<char, Character> Characters_gaegu;
	OBJModel particleObj;
	unsigned int cubemapTexture;

	RenderingSystem(SharedDataSystem* dataSys);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer(Camera camera, std::chrono::duration<double> timeLeft);
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

unsigned int loadCubemap(std::vector<std::string> faces);
void renderDeathScreen(unsigned int* VAO);

