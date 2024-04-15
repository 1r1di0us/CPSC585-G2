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
		tankHead, tankBody, tankWheel, bunny, train, toyBlock1, toyBlock2, particleObj;

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
	const unsigned int SCR_WIDTH = 1920;
	const unsigned int SCR_HEIGHT = 1080;

	// variables
	GLFWwindow* window;
	GLFWmonitor* primaryMonitor;
	int monitorWidth, monitorHeight, monitorRefresh;
	unsigned int VAO, VBO, textVAO, textVBO, skyVAO, skyVBO, particlesVAO, particlesVBO;
	Shader textShader;
	Shader shader;
	Shader skyBoxShader;
	Shader testShader; // testing it
	std::map<char, Character> Characters_gaegu;
	//OBJModel particleObj;
	unsigned int cubemapTexture;
	
	// camera offsets
	glm::vec3 offsetFromPlayer = glm::vec3(0.0f, 8.0f, 20.0f);
	glm::vec3 clipOffset = glm::vec3(0.0f, 2.0f, 3.0f);
	float transitionDuration = 2.0f; // Duration of transition in seconds
	float elapsedTime = 0.0f;
	bool isTransitioning = false; // Flag to track if transition is happening

	//helper functions
	
	//determines how far away the camera is from the an object with corner coords given
	float CamDistanceFromNearestClipSurface(glm::vec3 camPos);

	// functions
	RenderingSystem(SharedDataSystem* dataSys);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	void updateRenderer(Camera camera, std::chrono::duration<double> timeLeft, std::chrono::duration<double> deltaTime);
	//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	GLFWwindow* getWindow() const;
    
    // Generate particles at a given position
	void generateParticles(glm::vec3 position, int count, unsigned int tex, float velocity, float lowerLifetime, float upperLifetime);

	// Update particle positions and properties
	void particleUpdate(float deltaTime);

	// Render particles
	void particleRender(std::chrono::duration<double> deltaTime, glm::mat4 model);

	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 color;
		unsigned int texture;
		float size;
		float lifetime;
	};

	std::vector<Particle> particles;
	unsigned int particleVAO, particleVBO;
	Shader particleShader;
	void initParticlesVAO();

	unsigned int player1Texture, player2Texture, player3Texture, player4Texture, player5Texture, playerInvincibleTexture, redTexture,
		menuPlay, menuControls, menuQuit, controlsMenu, pauseMenuContinue, pauseMenuQuit,
		resultsP1, resultsP2, resultsP3, resultsP4, resultsP5, resultsTie,
		planeTexture, gunMetalTexture, parryTexture, ammoPowerupTexture, projectileSpeedPowerupTexture, projectileSizePowerupTexture, armourPowerupTexture, particleExplosionTexture, particleSmokeTexture;



	// Convert ratios to pixel coordinates
	std::pair<float, float> convertToPixels(float xRatio, float yRatio);
	void toggleFullscreen(GLFWwindow* window);

};

//void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO);
unsigned int loadCubemap(std::vector<std::string> faces);
void renderDeathScreen(unsigned int* VAO);

