#include "RenderingSystem.h">

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadCubemap(std::vector<std::string> faces);
bool checkCollision(glm::vec3 cameraPos, PxVec2 bottomLeft, PxVec2 topRight); 
bool checkCollisionMap(glm::vec3 cameraPos, PxVec3 map);

std::map<char, Character> Characters_gaegu;
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


glm::mat4 applyQuaternionToMatrix(const glm::mat4& matrix, const glm::quat& quaternion);
glm::mat4 applyQuaternionToMatrix(const glm::mat4& matrix, const glm::quat& quaternion) {
	// Extract the upper-left 3x3 rotation submatrix from the original matrix
	glm::mat3 rotationMatrix = glm::mat3(matrix);

	// Apply quaternion rotation to the rotation submatrix
	rotationMatrix = glm::mat3_cast(quaternion) * rotationMatrix;

	// Create a new matrix with the rotated rotation submatrix and original translation
	glm::mat4 rotatedMatrix = glm::mat4(rotationMatrix);
	rotatedMatrix[3] = matrix[3];  // Preserve the original translation

	return rotatedMatrix;
}

// constructor
RenderingSystem::RenderingSystem(SharedDataSystem* dataSys) {

	this->dataSys = dataSys;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // Enable window decorations (title bar, border)
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	primaryMonitor = glfwGetPrimaryMonitor();
	if (!primaryMonitor) {
		// Handle error
		glfwTerminate();
		return;
	}

	// Get the video mode of the primary monitor
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	if (!mode) {
		glfwTerminate();
		return;
	}

	// Extract width and height from the video mode
	monitorWidth = mode->width;
	monitorHeight = mode->height;
	monitorRefresh = mode->refreshRate;

	// windowed fullscreen, cuts off some of the text though
	//glfwGetMonitorWorkarea(primaryMonitor, NULL, NULL, &monitorWidth, &monitorHeight);
	//window = glfwCreateWindow(monitorWidth, monitorHeight, "Commander Paperchild Scrapyard Challenge", NULL, NULL);

	// fullscreen fullscreen
	//window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Commander Paperchild Scrapyard Challenge", get, NULL);
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Commander Paperchild Scrapyard Challenge", NULL, NULL);


	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	stbi_set_flip_vertically_on_load(true); // to vertically flip the image

	// depth for 3d rendering
	glEnable(GL_DEPTH_TEST);

	// text shader
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// create and set textures
	planeTexture = generateTexture("assets/Textures/wood.jpg", true);
	//stbi_set_flip_vertically_on_load(true); // to vertically flip the image

	//cars
	player1Texture = generateTexture("assets/Textures/player1.jpg", true);
	player2Texture = generateTexture("assets/Textures/player2.jpg", true);
	player3Texture = generateTexture("assets/Textures/player3.jpg", true);
	player4Texture = generateTexture("assets/Textures/player4.jpg", true);
	player5Texture = generateTexture("assets/Textures/player5.jpg", true);
	playerInvincibleTexture = generateTexture("assets/Textures/playerInvincible.jpg", true);
	shieldTexture = generateTexture("assets/Textures/shield.jpg", true);

	//random
	redTexture = generateTexture("assets/Textures/red.jpg", true);
	gunMetalTexture = generateTexture("assets/Textures/gunMetal.jpg", true);
	parryTexture = generateTexture("assets/Textures/blueparry.jpg", true);

	//menu
	menuPlay = generateTexture("assets/Textures/UI/menuPlay.jpg", true);
	menuControls = generateTexture("assets/Textures/UI/menuControls.jpg", true);
	menuQuit = generateTexture("assets/Textures/UI/menuQuit.jpg", true);
	controlsMenu = generateTexture("assets/Textures/UI/controlsMenu.jpg", true);
	pauseMenuContinue = generateTexture("assets/Textures/UI/pauseMenuContinue.jpg", true);
	pauseMenuQuit = generateTexture("assets/Textures/UI/pauseMenuQuit.jpg", true);

	//results
	resultsP1 = generateTexture("assets/Textures/UI/resultsP1.jpg", true);
	resultsP2 = generateTexture("assets/Textures/UI/resultsP2.jpg", true);
	resultsP3 = generateTexture("assets/Textures/UI/resultsP3.jpg", true);
	resultsP4 = generateTexture("assets/Textures/UI/resultsP4.jpg", true);
	resultsP5 = generateTexture("assets/Textures/UI/resultsP5.jpg", true);
	resultsTie = generateTexture("assets/Textures/UI/resultsTie.jpg", true);

	//powerups
	ammoPowerupTexture = gunMetalTexture;
	projectileSpeedPowerupTexture = gunMetalTexture;
	projectileSizePowerupTexture = gunMetalTexture;
	armourPowerupTexture = gunMetalTexture;

	// geom shaders
	shader = Shader("src/shaders/vertex_shader.txt", "src/shaders/fragment_shader.txt");
	skyBoxShader = Shader("src/shaders/skybox_vertex.txt", "src/shaders/skybox_fragment.txt");
	// setting the int has to do with something about the uniforms
	skyBoxShader.setInt("skybox", 0);


	//tank models
	tank = Model("./assets/Models/tank.obj");
	tankHead = Model("./assets/Models/tankHead.obj");
	tankBody = Model("./assets/Models/tankWithWheels.obj");
	tankWheel = Model("./assets/Models/tankWheel.obj");

	//static objects
	plane = Model("./assets/Models/emptyPlane.obj");
	bunny = Model("./assets/Models/toyBunny.obj");
	train = Model("./assets/Models/toyTrain.obj");
	toyBlock2 = Model("./assets/Models/toyBlocksHalf2.obj");
	toyBlock1 = Model("./assets/Models/toyBlocksHalf1.obj");

	//powerup models
	ammoPowerup = Model("./assets/Models/Powerups/ammo.obj");
	projectileSpeedPowerup = Model("./assets/Models/Powerups/projectileSpeed.obj");
	projectileSizePowerup = Model("./assets/Models/Powerups/projectileSize.obj");
	armourPowerup = Model("./assets/Models/Powerups/armour.obj");

	//other models
	projectile = Model("./assets/Models/ball.obj");
	particleObj = Model("./assets/Models/cube.obj");

	// SkyVAO Initialization
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// text VAO VBO Initialization
	textShader = Shader("src/shaders/vertex_shader_text.txt", "src/shaders/fragment_shader_text.txt");
	//glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	glm::mat4 textProjection = glm::ortho(0.0f, float(monitorWidth), 0.0f, float(monitorHeight));
	textShader.use();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

	Characters_gaegu = initFont("./assets/Fonts/Candy Beans.otf");
	initTextVAO(&textVAO, &textVBO);

	// loading skymap texture
	cubemapTexture = loadCubemap(faces);

    // Initialize particles VAO
    initParticlesVAO();
    // Load particle texture
	particleExplosionTexture = generateTexture("assets/Textures/fire.jpg", true);
	particleSmokeTexture = generateTexture("assets/Textures/playerInvincible.jpg", true);
    //particleShader = Shader("src/shaders/vertex_shader.txt", "src/shaders/fragment_shader.txt");
}



// to do: implement uniforms for obstacle rendering, or anything else that changes infrequently
void RenderingSystem::updateRenderer(Camera camera, std::chrono::duration<double> timeLeft, std::chrono::duration<double> deltaTime) {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// input
	processInput(window);
	toggleFullscreen(window);

	// render
	// clear the colorbuffer and the depthbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (!dataSys->inMenu) {
		// rendering text
		// Convert timeLeft to seconds
		int timeLeftInSeconds = static_cast<int>(timeLeft.count());

		//colors
		glm::vec3 color;
		glm::vec3 red = glm::vec3(1.0f, 0.5f, 0.5f);    // Adjusted to be darker
		glm::vec3 darkRed = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 blue = glm::vec3(0.7f, 0.7f, 1.0f);
		glm::vec3 green = glm::vec3(0.7f, 1.0f, 0.7f);
		glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.7f);
		glm::vec3 pink = glm::vec3(1.0f, 0.75f, 0.75f);  // Adjusted to be darker
		glm::vec3 white = glm::vec3(1.0f);
		glm::vec3 black = glm::vec3(0.0f);

		// text only if the player is alive
		if (dataSys->carInfoList[0].isAlive) {

			// Convert timeLeftInSeconds to string
			std::string timeLeftStr = "Time Left: " + std::to_string(timeLeftInSeconds);
			RenderText(textShader, textVAO, textVBO, timeLeftStr, 10.0f/800.0f * monitorWidth, 570.0f / 600.0f * monitorHeight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			//ammo count
			std::string ammoCount = "Ammo: " + std::to_string(dataSys->carInfoList[0].ammoCount);
			RenderText(textShader, textVAO, textVBO, ammoCount, 10.0f / 800.0f * monitorWidth, 30.0f / 600.0f * monitorHeight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			//scoreboard
			std::string score = "Score:";
			RenderText(textShader, textVAO, textVBO, score, 610.0f / 800.0f * monitorWidth, 570.0f / 600.0f * monitorHeight, 1.25f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			std::string parry = "Parry Available";
			if (dataSys->carInfoList[0].parryCooldownTimeLeft < 0) {
				RenderText(textShader, textVAO, textVBO, parry, 10.0f / 800.0f * monitorWidth, 60.0f / 600.0f * monitorHeight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);
			}

			for (int i = 0; i < dataSys->carInfoList.size(); i++) {
				if (i == 0) {
					color = red;
				}
				else if (i == 1) {
					color = blue;
				}
				else if (i == 2) {
					color = green;
				}
				else if (i == 3) {
					color = yellow;
				}
				else if (i == 4) {
					color = pink;
				}
				float yOffset = i * 30;
				std::string playerScore = "Player " + std::to_string(i + 1) + ": " + std::to_string(dataSys->carInfoList[i].score);
				RenderText(textShader, textVAO, textVBO, playerScore, (610.0f/800.f) * monitorWidth, ((540.0f - yOffset)/600.f * monitorHeight), 1.25f, color, Characters_gaegu);
			}

			//displays active powerups (temp until VFX)
			color = yellow;

			//coordinate vars
			//NEEDS TO EVENTUALLY BE BASED ON SCREEN SIZE
			float x = 5.0f/800.0f * monitorWidth;
			float y = 545.0f/600.0f * monitorHeight;

			std::string message;
			
			//player has armour
			if (dataSys->carInfoList[0].hasArmour) {
				message = "ARMOUR ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.75f, color, Characters_gaegu);
				y -= 20.0f;
			}
				
			//player has projectile size powerup
			if (dataSys->carInfoList[0].projectileSizeActiveTimeLeft > 0) {
				message = "PROJECTILE SIZE INCREASE ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.75f, color, Characters_gaegu);
				y -= 20.0f;
			}

			//player has projectile speed powerup
			if (dataSys->carInfoList[0].projectileSpeedActiveTimeLeft > 0) {
				message = "PROJECTILE SPEED INCREASE ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.75f, color, Characters_gaegu);
				y -= 20.0f;
			}

		}
		else {
			color = black;
			// death text
			textShader.use();
			std::string deathText1 = "You died!";
			std::string deathText2 = "Welcome to heaven!";
			std::string deathTimer = "Respawn in: " + std::to_string(static_cast<int>(dataSys->carInfoList[0].respawnTimeLeft));
			RenderText(textShader, textVAO, textVBO, deathText1, 350.0f/800.0f * monitorWidth, 500.0f/600.0f * monitorHeight, 1.0f, color, Characters_gaegu);
			RenderText(textShader, textVAO, textVBO, deathText2, 310.0f/800.0f * monitorWidth, 90.0f/600.0f * monitorHeight, 1.0f, color, Characters_gaegu);
			RenderText(textShader, textVAO, textVBO, deathTimer, 330.0f/800.0f * monitorWidth, 50.0f/600.0f * monitorHeight, 1.0f, color, Characters_gaegu);
			
		}

		// activate shader
		shader.use();

		// camera setup stuff/ 3d transformations
		glm::mat4 model = glm::mat4(1.0f);

		// view matrix
		glm::mat4 view = glm::mat4(1.0f);
		// note that we're translating the scene in the reverse direction of where we want to move
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -7.0f));

		// this should be the camera matrix
		glm::mat4 projection = glm::mat4(1.0f);
		//projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		
		projection = glm::perspective(glm::radians(45.0f), (float)monitorWidth / (float)monitorHeight, 0.1f, 300.0f);

		// getting the car position and rotation
		glm::vec3 playerPos = dataSys->carInfoList[0].entity->transform->pos;
		glm::quat playerRot = dataSys->carInfoList[0].entity->transform->rot;
		//std::cout << playerPos.x << ":" << playerPos.y << ":" << playerPos.z << std::endl;

		// Check for collision with obstacles
		bool collisionDetected = false;

		if (dataSys->useBirdsEyeView >= 1 && dataSys->useBirdsEyeView < 3) {
			// Bird's eye view
			glm::vec3 cameraPosition = glm::vec3(0.0f, 200.0f, 0.0f);
			glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

			view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		}
		else {
			// Original view
			float distanceFromObstacle;

			if (!dataSys->carInfoList[0].isAlive) {
				offsetFromPlayer = glm::vec3(0.0f, 2.0f, 10.0f);
			}

			//checks each obstacle and map for clipping
			float distance = CamDistanceFromNearestClipSurface(camera.Position);
			
			//if the camera is clipping
			if (distance < dataSys->CAMERA_CLIP_DISTANCE) {

				camera.Position = playerPos + dataSys->getCamRotMat() * offsetFromPlayer; //we rotate camera with getCamRotMat
			}
			
			glm::vec3 lookAtPoint = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
			view = glm::lookAt(camera.Position, lookAtPoint, camera.Up);
		}

		// rendering player car
		shader.use();
		model = glm::translate(model, playerPos);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f)); // make it look forward
		model = applyQuaternionToMatrix(model, playerRot);

		// sending our matrixes to the shader
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		//updating the shoot direction of the player car to be the direction of the camera
		auto test = dataSys->getCamRotMat() * glm::vec3(0, 0, -1);
		dataSys->carInfoList[0].shootDir.x = test.x;
		dataSys->carInfoList[0].shootDir.z = test.z;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, player1Texture);

		// binding textures
		if (dataSys->carInfoList[0].parryActiveTimeLeft > 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, parryTexture);
			if (!dataSys->carInfoList[0].parryParticles) {
				generateParticles(playerPos, 15, parryTexture, 30, 1, 3);
				dataSys->carInfoList[0].parryParticles = true;
			}
		}
		else if (dataSys->carInfoList[0].iFramesLeft > 0) {
			glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, playerInvincibleTexture);
		}
		else if (dataSys->carInfoList[0].hasArmour) {
			glBindTexture(GL_TEXTURE_2D, shieldTexture);
		}
		else {
			dataSys->carInfoList[0].parryParticles = false;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, player1Texture);
		}
		tankBody.Draw(shader);

		if (dataSys->carInfoList[0].shotBullet) {
			glm::vec3 smokePosition = dataSys->carInfoList[0].entity->transform->pos;
			smokePosition.z += 2;
			smokePosition.y += 2;
			generateParticles(smokePosition, 15, particleSmokeTexture, 3, 25, 40);
			dataSys->carInfoList[0].shotBullet = false;
		}

		// tank head
		glm::mat4 tankHeadModel = glm::mat4(1.0f);
		glm::vec3 shootDir = glm::normalize(glm::vec3(dataSys->carInfoList[0].shootDir.x, dataSys->carInfoList[0].shootDir.y, dataSys->carInfoList[0].shootDir.z));
		float angle = atan2(shootDir.x, shootDir.z);

		tankHeadModel = glm::translate(tankHeadModel, playerPos + dataSys->ConvertPXVec3ToGLM(dataSys->TurretOffsetVector(&dataSys->carInfoList[0])));
		tankHeadModel = glm::rotate(tankHeadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated
		tankHeadModel = glm::rotate(tankHeadModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", tankHeadModel);
		tankHead.Draw(shader);

		// if player is alive, draw the scene
		if (dataSys->carInfoList[0].isAlive) {

			//render all projectiles
			for (CarInfo carInfo : dataSys->carInfoList) {

				//get the rigid dynamic from the entity
				auto carRigidDynamic = carInfo.entity->collisionBox;

				//go through each projectile that a car has shot and render it
				for (int j = 0; j < dataSys->carProjectileRigidDynamicDict[carRigidDynamic].size(); j++) {

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, redTexture);

					model = glm::mat4(1.0f);
					model = glm::translate(model, dataSys->GetEntityFromRigidDynamic(dataSys->carProjectileRigidDynamicDict[carRigidDynamic][j])->transform->getPos());
					model = glm::scale(model, glm::vec3(dataSys->carProjectileRigidDynamicDict[carRigidDynamic][j]->getWorldBounds().getExtents().x));

					shader.setMat4("model", model);
					projectile.Draw(shader);

				}
			}

			//rendering all other entities starting at the size of the static list + 1 (+1 isnt needed cause of index 0) (BW added +1, caused rendering issues)
			for (int i = 0; i < dataSys->entityList.size(); i++) {

				switch (dataSys->entityList[i].physType) {

				case (PhysicsType::CAR):

					//gets the car info struct of the car
					CarInfo* carInfo;

					//is the car alive? -> render it
					if (dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->isAlive) {

						carInfo = dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]));

						//not sure if needed but keeping for now
						if (carInfo->entity->name == "car1") {
							break;
						}

						if (carInfo->iFramesLeft > 0) {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, playerInvincibleTexture);
						}
						else if (carInfo->hasArmour) {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, shieldTexture);
						}
						//different colors for different cars
						else if (carInfo->entity->name == "car2") {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, player2Texture);
						}
						else if (carInfo->entity->name == "car3") {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, player3Texture);
						}
						else if (carInfo->entity->name == "car4") {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, player4Texture);
						}
						else if (carInfo->entity->name == "car5") {
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, player5Texture);
						}

						// tank body
						model = glm::mat4(1.0f);
						model = glm::translate(model, dataSys->entityList[i].transform->getPos());
						// make it look forward, double check if this is for the body
						model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
						model = applyQuaternionToMatrix(model, dataSys->entityList[i].transform->getRot());
						shader.setMat4("model", model);
						tankBody.Draw(shader);

						// tank head
						glm::mat4 tankHeadModel = glm::mat4(1.0f);
						glm::vec3 shootDir = glm::normalize(glm::vec3(carInfo->shootDir.x, carInfo->shootDir.y, carInfo->shootDir.z));
						float angle = atan2(shootDir.x, shootDir.z);

						tankHeadModel = glm::translate(tankHeadModel, dataSys->entityList[i].transform->getPos() + dataSys->ConvertPXVec3ToGLM(dataSys->TurretOffsetVector(carInfo)));
						tankHeadModel = glm::rotate(tankHeadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated
						tankHeadModel = glm::rotate(tankHeadModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
						shader.setMat4("model", tankHeadModel);
						tankHead.Draw(shader);

						// Render smoke
						if (carInfo->shotBullet) {
							glm::vec3 smokePosition = carInfo->entity->transform->pos;
							smokePosition.z += 2;
							smokePosition.y += 2;
							generateParticles(smokePosition, 15, particleSmokeTexture, 3, 25, 40);
							carInfo->shotBullet = false;
						}
					}
                    else if (!dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->isAlive) {
                        if (!dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->exploded) {
                            CarInfo* carInfoStruct = dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]));
                            glm::vec3 carPos = carInfoStruct->entity->transform->pos;
                            carPos.y -= 150.0f;
                            generateParticles(carPos, 25, particleExplosionTexture, 3, 25, 50);
                            dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->exploded = true;
                        }
                    }

					break;
				case (PhysicsType::POWERUP):
					
					model = glm::mat4(1.0f);
					model = glm::translate(model, dataSys->entityList[i].transform->getPos());

					//changes based on powerup type
					switch (dataSys->GetPowerupInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->powerupType) {
					case PowerupType::AMMO:

						model = glm::scale(model, glm::vec3(0.3f));

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, ammoPowerupTexture);

						shader.setMat4("model", model);
						ammoPowerup.Draw(shader);

						break;
					case PowerupType::PROJECTILESPEED:

						model = glm::translate(model, glm::vec3(0, -1.3, 0));
						model = glm::scale(model, glm::vec3(0.22f));

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, projectileSpeedPowerupTexture);

						shader.setMat4("model", model);
						projectileSpeedPowerup.Draw(shader);

						break;
					case PowerupType::PROJECTILESIZE:

						model = glm::translate(model, glm::vec3(0, 1.2, 0));
						model = glm::rotate(model, glm::half_pi<float>(), glm::vec3(0, 0, 1));
						model = glm::scale(model, glm::vec3(0.09f));

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, projectileSizePowerupTexture);

						shader.setMat4("model", model);
						projectileSizePowerup.Draw(shader);

						break;
					case PowerupType::ARMOUR:

						model = glm::scale(model, glm::vec3(0.095f));

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, armourPowerupTexture);

						shader.setMat4("model", model);
						armourPowerup.Draw(shader);

					case PowerupType::CARSPEED:

						break;
					default:
						printf("not possible for this powerup type to be rendered");
						break;
					}

					break;
				case (PhysicsType::STATIC):
					// render obstacles

					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
					model = glm::translate(model, dataSys->entityList[i].transform->getPos());
					shader.use();
					shader.setMat4("model", model);

					if (dataSys->entityList[i].name == "STATIC_0")
					{
						plane.Draw(shader);
					}
					else if (dataSys->entityList[i].name == "STATIC_1") {
						train.Draw(shader);
					}
					else if (dataSys->entityList[i].name == "STATIC_2") {
						bunny.Draw(shader);
					}
					else if (dataSys->entityList[i].name == "STATIC_3") {
						toyBlock1.Draw(shader);
					}
					else if ( dataSys->entityList[i].name == "STATIC_4") {
						toyBlock2.Draw(shader);
					}

					break;
				default:

					break;
				}
			}
		}
		if (!dataSys->inMenu && !dataSys->inGameMenu && !dataSys->inResults) {
			particleRender(deltaTime, model);
		}

		// skybox rendering, needs to be at the end of rendering
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		skyBoxShader.use();
		skyBoxShader.setMat4("projection", projection);
		glm::mat4 skyView = glm::mat4(glm::mat3(view));
		skyBoxShader.setMat4("view", skyView);
		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
		glDepthMask(GL_TRUE);
	}

	// Setup UI if necessary
	if (dataSys->inMenu || dataSys->inResults || dataSys->inGameMenu) {
		GLuint fboId = 0;
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);

		if (dataSys->inMenu) {
			if (dataSys->inControlsMenu) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, controlsMenu, 0);
			}
			else if (dataSys->menuOptionIndex == 0) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, menuPlay, 0);
			}
			else if (dataSys->menuOptionIndex == 1) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, menuControls, 0);
			}
			else if (dataSys->menuOptionIndex == 2) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, menuQuit, 0);
			}
		}
		else if (dataSys->inResults) {
			if (dataSys->tieGame) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsTie, 0);
			}
			else {
				if (dataSys->winningPlayer == 0) {
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP1, 0);
				}
				else if (dataSys->winningPlayer == 1) {
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP2, 0);
				}
				else if (dataSys->winningPlayer == 2) {
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP3, 0);
				}
				else if (dataSys->winningPlayer == 3) {
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP4, 0);
				}
				else {
					glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP5, 0);
				}
			}
		}
		else if (dataSys->inGameMenu) {
			if (dataSys->ingameOptionIndex == 0) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pauseMenuContinue, 0);
			}
			else if (dataSys->ingameOptionIndex == 1) {
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pauseMenuQuit, 0);
			}
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	// swap buffers and poll IO events
	glfwSwapBuffers(window);
	glfwPollEvents();
}


void renderObject(const OBJModel& model, unsigned int* VAO) {
    glBindVertexArray(*VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void RenderingSystem::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void RenderingSystem::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

GLFWwindow* RenderingSystem::getWindow() const {
	return window;
}

void RenderingSystem::generateParticles(glm::vec3 position, int count, unsigned int tex, float velocity, float lowerLifetime, float upperLifetime) {
    for (int i = 0; i < count; ++i) {
        Particle particle;
        particle.position = position;
        particle.velocity = glm::sphericalRand(velocity); // Random velocity
        particle.color = glm::vec4(1.0f); // White color
        particle.size = glm::linearRand(0.1f, 0.4f); // Random size
        particle.lifetime = glm::linearRand(lowerLifetime, upperLifetime); // Random lifetime
		particle.texture = tex;
        particles.push_back(particle);
    }
}

void RenderingSystem::particleUpdate(float deltaTime) {
    for (auto& particle : particles) {
        // Update particle position based on velocity
        particle.position += particle.velocity * deltaTime * float(0.05);

        // Decrease particle lifetime
        particle.lifetime -= deltaTime;

        // If particle's lifetime is over, remove it
        if (particle.lifetime <= 0.0f) {
            particles.erase(std::remove_if(particles.begin(), particles.end(),
                [](const Particle& p) { return p.lifetime <= 0.0f; }), particles.end());
        }
    }
}

void RenderingSystem::particleRender(std::chrono::duration<double> deltaTime, glm::mat4 model) {
    for (auto& particle : particles) {
        particleUpdate(static_cast<float>(deltaTime.count()));
        // Activate particle shader and set camera matrix
        // Bind particle texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particle.texture);

        //// Render particles
        //glBindVertexArray(particleVAO);
        //glDrawArrays(GL_POINTS, 0, particles.size());
        model = glm::mat4(1.0f);
        model = glm::translate(model, particle.position);
        glm::vec3 scaleFactors(particle.size, particle.size, particle.size); // Scaling factors for x, y, and z axes
        model = glm::scale(model, scaleFactors);
        shader.setMat4("model", model);
        //renderObject(particleObj, &particlesVAO);
		particleObj.Draw(shader);
    }
}

void RenderingSystem::initParticlesVAO() {
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

    // Reserve space for particles (initially empty)
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Enable vertex attribute arrays
    glEnableVertexAttribArray(0); // Position
    glEnableVertexAttribArray(1); // Color
    glEnableVertexAttribArray(2); // Size

    // Position attribute (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    // Color attribute (vec4)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    // Size attribute (float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

    glBindVertexArray(0); // Unbind VAO
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(false); // images flipped

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

// Convert ratios to pixel coordinates
std::pair<float, float> RenderingSystem::convertToPixels(float xRatio, float yRatio) {
	float pixelX = xRatio * monitorWidth;
	float pixelY = yRatio * monitorHeight;
	return std::make_pair(pixelX, pixelY);
}

void RenderingSystem::toggleFullscreen(GLFWwindow* window) {
	if (dataSys->useWindowFullscreen) {
		// Switch to fullscreen mode
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (mode != nullptr) {
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			// Adjust viewport to match window size
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
		}
	}
	else {
		// Switch to windowed mode with a size of 800 x 600
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowMonitor(window, NULL, 0, 20, SCR_WIDTH, SCR_HEIGHT, GLFW_DONT_CARE);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}
}

float RenderingSystem::CamDistanceFromNearestClipSurface(glm::vec3 cameraPos) {

	//variables
	float distance = FLT_MAX;
	glm::vec2 bottomLeftVec2;
	glm::vec2 topRightVec2;

	//check against the map first (optimization)
	PxVec3 mapDimensions = dataSys->entityList[0].collisionBox->getWorldBounds().getDimensions();

	int width = mapDimensions.x / 2;
	int depth = mapDimensions.z / 2;

	// Define the dimensions of the box and the buffer
	float boxWidth = width;
	float boxDepth = depth;
	float buffer = 10.0f; // Buffer size

	//is the camera within the buffered box?
	distance =
		std::min(std::abs(cameraPos.x - (-boxWidth + buffer)),
			std::min(std::abs(cameraPos.x - (boxWidth - buffer)),
				std::min(std::abs(cameraPos.z - (-boxDepth + buffer)), std::abs(cameraPos.z - (boxDepth - buffer)))
			));

	//why bother checking the rest if the map is too close
	if (distance < dataSys->CAMERA_CLIP_DISTANCE) {
		return distance;
	}

	//for comparisons
	float minDistanceOfObstacle;

	//go through each obstacle EXCLUDING map
	for (int i = 0; i < dataSys->obstacleMapSquareList.size(); i++) {

		//setting the variables for math correctly
		bottomLeftVec2 = glm::vec2(dataSys->obstacleMapSquareList[i].bottomLeft.x, dataSys->obstacleMapSquareList[i].bottomLeft.y);
		topRightVec2 = glm::vec2(dataSys->obstacleMapSquareList[i].topRight.x, dataSys->obstacleMapSquareList[i].topRight.y);
	
		//finding the absolute min distance from all obstacles
		minDistanceOfObstacle =
			std::min(std::abs(cameraPos.x - bottomLeftVec2.x),
				std::min(std::abs(cameraPos.x - topRightVec2.x),
					std::min(std::abs(cameraPos.z - bottomLeftVec2.y), std::abs(cameraPos.z - topRightVec2.y))
				));

		if (minDistanceOfObstacle < distance) {
			distance = minDistanceOfObstacle;
			
			//threshold of do something about it
			if (distance < dataSys->CAMERA_CLIP_DISTANCE) {
				return distance;
			}
		}

	}

	return distance;
}

bool checkCollisionMap(glm::vec3 cameraPos, PxVec3 mapDimensions) {
	int width = mapDimensions.x / 2;
	int depth = mapDimensions.z / 2;

	// Define the dimensions of the box and the buffer
	float boxWidth = width;
	float boxDepth = depth;
	float buffer = 10.0f; // Buffer size

	// Check if the camera position satisfies the specified conditions
	bool betweenBoxes =
		(cameraPos.x < (-boxWidth + buffer) || cameraPos.x > (boxWidth - buffer)) || // Camera position is outside the box in the x-direction within the buffer
		(cameraPos.z < (-boxDepth + buffer) || cameraPos.z > (boxDepth - buffer));    // Camera position is outside the box in the z-direction within the buffer

	return betweenBoxes;
}