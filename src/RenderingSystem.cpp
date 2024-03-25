#include "RenderingSystem.h">

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadCubemap(std::vector<std::string> faces);

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


unsigned int player1Texture, player2Texture, player3Texture, player4Texture, player5Texture, redTexture, menuPlay, menuControls, menuQuit, controlsMenu, pauseMenuContinue, pauseMenuQuit, resultsP1, resultsP2, resultsP3, resultsP4, resultsP5, resultsTie, planeTexture, gunMetalTexture, rainbow;

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
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
	player1Texture = generateTexture("assets/Textures/player1.jpg", true);
	player2Texture = generateTexture("assets/Textures/player2.jpg", true);
	player3Texture = generateTexture("assets/Textures/player3.jpg", true);
	player4Texture = generateTexture("assets/Textures/player4.jpg", true);
	player5Texture = generateTexture("assets/Textures/player5.jpg", true);
	redTexture = generateTexture("assets/Textures/red.jpg", true);
	gunMetalTexture = generateTexture("assets/Textures/gunMetal.jpg", true);
	menuPlay = generateTexture("assets/Textures/UI/menuPlay.jpg", true);
	menuControls = generateTexture("assets/Textures/UI/menuControls.jpg", true);
	menuQuit = generateTexture("assets/Textures/UI/menuQuit.jpg", true);
	controlsMenu = generateTexture("assets/Textures/UI/controlsMenu.jpg", true);
	pauseMenuContinue = generateTexture("assets/Textures/UI/pauseMenuContinue.jpg", true);
	pauseMenuQuit = generateTexture("assets/Textures/UI/pauseMenuQuit.jpg", true);
	resultsP1 = generateTexture("assets/Textures/UI/resultsP1.jpg", true);
	resultsP2 = generateTexture("assets/Textures/UI/resultsP2.jpg", true);
	resultsP3 = generateTexture("assets/Textures/UI/resultsP3.jpg", true);
	resultsP4 = generateTexture("assets/Textures/UI/resultsP4.jpg", true);
	resultsP5 = generateTexture("assets/Textures/UI/resultsP5.jpg", true);
	resultsTie = generateTexture("assets/Textures/UI/resultsTie.jpg", true);
	rainbow = generateTexture("assets/Textures/whiteparry.jpg", true);

	// geom shaders
	shader = Shader("src/shaders/vertex_shader.txt", "src/shaders/fragment_shader.txt");
	skyBoxShader = Shader("src/shaders/skybox_vertex.txt", "src/shaders/skybox_fragment.txt");
	// setting the int has to do with something about the uniforms
	skyBoxShader.setInt("skybox", 0);


	// loading in the models
	plane = Model("./assets/Models/emptyPlane.obj");
	projectile = Model("./assets/Models/ball.obj");
	tank = Model("./assets/Models/tank.obj");
	powerup = Model("./assets/Models/ball.obj");
	tankHead = Model("./assets/Models/tankHead.obj");
	tankBody = Model("./assets/Models/tankBody.obj");
	tankWheel = Model("./assets/Models/tankWheel.obj");
	bunny = Model("./assets/Models/toyBunny.obj");
	train = Model("./assets/Models/toyTrain.obj");
	toyBlock2 = Model("./assets/Models/toyBlocksHalf2.obj");
	toyBlock1 = Model("./assets/Models/toyBlocksHalf1.obj");

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
	glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	textShader.use();
	glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

	Characters_gaegu = initFont("./assets/Fonts/Candy Beans.otf");
	initTextVAO(&textVAO, &textVBO);

	// loading skymap texture
	cubemapTexture = loadCubemap(faces);
}



// to do: implement uniforms for obstacle rendering, or anything else that changes infrequently
void RenderingSystem::updateRenderer(Camera camera, std::chrono::duration<double> timeLeft) {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	// input
	processInput(window);

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
			RenderText(textShader, textVAO, textVBO, timeLeftStr, 10.0f, 570.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			//ammo count
			std::string ammoCount = "Ammo: " + std::to_string(dataSys->carInfoList[0].ammoCount);
			RenderText(textShader, textVAO, textVBO, ammoCount, 10.0f, 10.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			//scoreboard
			std::string score = "Score:";
			RenderText(textShader, textVAO, textVBO, score, 610.0f, 570.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

			std::string parry = "Parry Available";
			if (dataSys->carInfoList[0].parryCooldownTimeLeft < 0) {
				RenderText(textShader, textVAO, textVBO, parry, 10.0f, 40.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);
			}
			else
			{
				std::string parryTime = "Parry Cooldown: " + std::to_string(static_cast<int>(dataSys->carInfoList[0].parryCooldownTimeLeft));
				RenderText(textShader, textVAO, textVBO, parryTime, 10.0f, 40.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);
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
				RenderText(textShader, textVAO, textVBO, playerScore, 610.0f, 540.0f - yOffset, 0.75f, color, Characters_gaegu);
			}

			//displays active powerups (temp until VFX)
			color = yellow;

			//coordinate vars
			//NEEDS TO EVENTUALLY BE BASED ON SCREEN SIZE
			float x = 5.0f;
			float y = 545.0f;

			std::string message;
			
			//player has armour
			if (dataSys->carInfoList[0].hasArmour) {
				message = "ARMOUR ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.35f, color, Characters_gaegu);
				y -= 20.0f;
			}
				
			//player has projectile size powerup
			if (dataSys->carInfoList[0].projectileSizeActiveTimeLeft > 0) {
				message = "PROJECTILE SIZE INCREASE ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.35f, color, Characters_gaegu);
				y -= 20.0f;
			}

			//player has projectile speed powerup
			if (dataSys->carInfoList[0].projectileSpeedActiveTimeLeft > 0) {
				message = "PROJECTILE SPEED INCREASE ACTIVE";
				RenderText(textShader, textVAO, textVBO, message, x, y, 0.35f, color, Characters_gaegu);
				y -= 20.0f;
			}

		}
		else {

			color = darkRed;
			// death text
			textShader.use();
			std::string deathText1 = "You died!";
			std::string deathText2 = "Welcome to heaven!";
			RenderText(textShader, textVAO, textVBO, deathText1, 310.0f, 500.0f, 1.0f, color, Characters_gaegu);
			RenderText(textShader, textVAO, textVBO, deathText2, 190.0f, 90.0f, 1.0f, color, Characters_gaegu);
			// render the 2d screen if they are dead?
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
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		// getting the car position and rotation
		glm::vec3 playerPos = dataSys->carInfoList[0].entity->transform->pos;
		glm::quat playerRot = dataSys->carInfoList[0].entity->transform->rot;
		//std::cout << playerPos.x << ":" << playerPos.y << ":" << playerPos.z << std::endl;

		if (dataSys->useBirdsEyeView >= 1 && dataSys->useBirdsEyeView < 3) {
			// Bird's eye view
			glm::vec3 cameraPosition = glm::vec3(0.0f, 200.0f, 0.0f);
			glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

			view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
		}
		else {
			// Original view
			glm::vec3 offsetFromPlayer;
			if (dataSys->carInfoList[0].isAlive) {
				offsetFromPlayer = glm::vec3(0.0f, 8.0f, 20.0f);
			}
			else {
				offsetFromPlayer = glm::vec3(0.0f, 2.0f, 10.0f);
			}
			camera.Position = playerPos + dataSys->getCamRotMat() * offsetFromPlayer; //we rotate camera with getCamRotMat
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
			glBindTexture(GL_TEXTURE_2D, rainbow);
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, player1Texture);
		}
		tankBody.Draw(shader);

		//glm::vec3 frontRight = glm::vec3(-1.245f, 0.321f, 2.563f);
		//glm::vec3 frontLeft = glm::vec3(1.245f, 0.321f, 2.563f);
		//glm::vec3 backRight = glm::vec3(-1.245f, 0.321f, 0.1f);
		//glm::vec3 backLeft = glm::vec3(1.245f, 0.321f, 0.1f);

		//glm::mat4 wheelModel = glm::translate(glm::mat4(1.0f), playerPos);
		//wheelModel = glm::rotate(wheelModel, glm::radians(90.0f), glm::vec3(0, 1.0f, 0));
		//wheelModel = glm::translate(wheelModel, backRight);
		//shader.setMat4("model", wheelModel);
		//tankWheel.Draw(shader);
		//wheelModel = glm::translate(glm::mat4(1.0f), playerPos);
		//wheelModel = glm::translate(wheelModel, backLeft);
		//printf("wheel location: %f, %f, %f\n", dataSys->carInfoList[0].carWheelInfo.backRightPos.x, dataSys->carInfoList[0].carWheelInfo.backRightPos.y, dataSys->carInfoList[0].carWheelInfo.backRightPos.z);
		//shader.setMat4("model", wheelModel);
		//tankWheel.Draw(shader);

		// tank head
		glm::mat4 tankHeadModel = glm::mat4(1.0f);
		glm::vec3 shootDir = glm::normalize(glm::vec3(dataSys->carInfoList[0].shootDir.x, dataSys->carInfoList[0].shootDir.y, dataSys->carInfoList[0].shootDir.z));
		float angle = atan2(shootDir.x, shootDir.z);
		tankHeadModel = glm::translate(tankHeadModel, playerPos);
		tankHeadModel = glm::rotate(tankHeadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated
		tankHeadModel = glm::rotate(tankHeadModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", tankHeadModel);
		tankHead.Draw(shader);

		//// front wheels
		//glm::vec3 wheelDir = glm::normalize(glm::vec3(dataSys->carInfoList[0].carWheelInfo.wheelForwardDir.x,
		//	dataSys->carInfoList[0].carWheelInfo.wheelForwardDir.y,
		//	dataSys->carInfoList[0].carWheelInfo.wheelForwardDir.z));
		// front wheels
		//glm::vec3 wheelDir = glm::normalize(glm::vec3(dataSys->carInfoList[0].wheelForwardDir.x, dataSys->carInfoList[0].wheelForwardDir.y, dataSys->carInfoList[0].wheelForwardDir.z));

		//angle = atan2(wheelDir.x, wheelDir.z);
		//glm::mat4 wheelModel = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
		//wheelModel = glm::translate(wheelModel, playerPos);
		//wheelModel = glm::rotate(wheelModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated

		//shader.setMat4("model", tankHeadModel);
		//wheelModel = glm::translate(glm::mat4(1.0f), playerPos);
		//std::cout << dataSys->carInfoList[0].carWheelInfo.frontRightPos.x << std::endl;
		//glm::vec3 wheelGlobal = glm::normalize(glm::vec3(dataSys->carInfoList[0].carWheelInfo.frontRightPos.x, dataSys->carInfoList[0].carWheelInfo.frontRightPos.y, dataSys->carInfoList[0].carWheelInfo.frontRightPos.z));
		//shader.setMat4("model", wheelModel);
		////tankWheel.Draw(shader);
		//wheelModel = glm::translate(glm::mat4(1.0f), playerPos);
		//wheelModel = glm::translate(wheelModel, glm::normalize(glm::vec3(dataSys->carInfoList[0].carWheelInfo.frontLeftPos.x, dataSys->carInfoList[0].carWheelInfo.frontLeftPos.y, dataSys->carInfoList[0].carWheelInfo.frontLeftPos.z)));
		//shader.setMat4("model", wheelModel);
		////tankWheel.Draw(shader);


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

					//if the car has size powerup active
					if (carInfo.projectileSizeActiveTimeLeft > 0) {

						model = glm::scale(model, glm::vec3(dataSys->PROJECTILE_SIZE_POWERUP_STRENGTH));
					}

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

						//different colors for different cars
						if (carInfo->entity->name == "car2") {
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
						//tankWheel.Draw(shader);
						//tankWheel.Draw(shader);

						//NOTE: does car have shields?

						// tank head
						glm::mat4 tankHeadModel = glm::mat4(1.0f);
						glm::vec3 shootDir = glm::normalize(glm::vec3(carInfo->shootDir.x, carInfo->shootDir.y, carInfo->shootDir.z));
						float angle = atan2(shootDir.x, shootDir.z);
						tankHeadModel = glm::translate(tankHeadModel, dataSys->entityList[i].transform->getPos());
						tankHeadModel = glm::rotate(tankHeadModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated
						tankHeadModel = glm::rotate(tankHeadModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
						shader.setMat4("model", tankHeadModel);
						tankHead.Draw(shader);

						// front wheels
						glm::mat4 tankWheelRight = glm::mat4(1.0f);
						glm::mat4 tankWheelLeft = glm::mat4(1.0f);
						glm::vec3 wheelDir = glm::normalize(glm::vec3(carInfo->wheelForwardDir.x, carInfo->wheelForwardDir.y, carInfo->wheelForwardDir.z));
						angle = atan2(wheelDir.x, wheelDir.z);
						glm::mat4 wheelModel = glm::translate(glm::mat4(1.0f), dataSys->entityList[i].transform->getPos());
						//wheelModel = glm::rotate(wheelModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // the tank head model needs to be rotated
						wheelModel = glm::rotate(wheelModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
						shader.setMat4("model", tankHeadModel);
						//tankWheel.Draw(shader);
						//tankWheel.Draw(shader);
					}

					break;
				case (PhysicsType::PROJECTILE):					

					break;
				case (PhysicsType::POWERUP):
					
					model = glm::mat4(1.0f);
					model = glm::translate(model, dataSys->entityList[i].transform->getPos());

					//changes based on powerup type
					switch (dataSys->GetPowerupInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->powerupType) {
					case PowerupType::AMMO:

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, gunMetalTexture);

						shader.setMat4("model", model);
						projectile.Draw(shader);

						break;
					case PowerupType::PROJECTILESPEED:

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, player5Texture);

						shader.setMat4("model", model);
						projectile.Draw(shader);

						break;
					case PowerupType::PROJECTILESIZE:

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, player2Texture);

						shader.setMat4("model", model);
						projectile.Draw(shader);

						break;
					case PowerupType::ARMOUR:

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, player4Texture);

						shader.setMat4("model", model);
						projectile.Draw(shader);

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
					model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
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