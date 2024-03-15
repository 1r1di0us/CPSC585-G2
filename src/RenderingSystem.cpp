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


unsigned int player1Texture, player2Texture, player3Texture, player4Texture, player5Texture, redTexture, menuPlay, menuControls, menuQuit, controlsMenu, resultsP1, resultsP2, resultsP3, resultsP4, resultsP5, resultsTie, planeTexture;

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
    planeTexture = generateTexture("src/Textures/wood.jpg", true);
    player1Texture = generateTexture("src/Textures/player1.jpg", true);
    player2Texture = generateTexture("src/Textures/player2.jpg", true);
    player3Texture = generateTexture("src/Textures/player3.jpg", true);
    player4Texture = generateTexture("src/Textures/player4.jpg", true);
    player5Texture = generateTexture("src/Textures/player5.jpg", true);
    redTexture = generateTexture("src/Textures/red.jpg", true);
    menuPlay = generateTexture("src/Textures/UI/menuPlay.jpg", true);
    menuControls = generateTexture("src/Textures/UI/menuControls.jpg", true);
    menuQuit = generateTexture("src/Textures/UI/menuQuit.jpg", true);
    controlsMenu = generateTexture("src/Textures/UI/controlsMenu.jpg", true);
    resultsP1 = generateTexture("src/Textures/UI/resultsP1.jpg", true);
    resultsP2 = generateTexture("src/Textures/UI/resultsP2.jpg", true);
    resultsP3 = generateTexture("src/Textures/UI/resultsP3.jpg", true);
    resultsP4 = generateTexture("src/Textures/UI/resultsP4.jpg", true);
    resultsP5 = generateTexture("src/Textures/UI/resultsP5.jpg", true);
    resultsTie = generateTexture("src/Textures/UI/resultsTie.jpg", true);

    // geom shaders
    shader = Shader("src/vertex_shader.txt", "src/fragment_shader.txt");
    skyBoxShader = Shader("src/skybox_vertex.txt", "src/skybox_fragment.txt");
    // setting the int has to do with something about the uniforms
    skyBoxShader.setInt("skybox", 0);

    //ourShader = Shader("src/model_loading_vertex.txt", "src/model_loading_fragment.txt");

    bedModel = Model("./assets/Models/bed_double_A1.obj");
    funkyCube = Model("./assets/Models/funky_cube.obj");
    plane = Model("./assets/Models/planeHugeWithWalls.obj");
    projectile = Model("./assets/Models/ball.obj");
    tank = Model("./assets/Models/tank.obj");
    powerup = Model("./assets/Models/bed_double_A1.obj");

    //std::cout << bedModel.meshes.at(0).vertices.size() << std::endl;

    initVAO(skyboxVertices, sizeof(skyboxVertices), &skyVAO, &skyVBO);


    textShader = Shader("src/vertex_shader_text.txt", "src/fragment_shader_text.txt");
    glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    Characters_gaegu = initFont("./assets/Candy Beans.otf");
    initTextVAO(&textVAO, &textVBO);

    // loading skymap texture
    cubemapTexture = loadCubemap(faces);
}


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

        // Convert timeLeftInSeconds to string
        std::string timeLeftStr = "Time Left: " + std::to_string(timeLeftInSeconds);
        RenderText(textShader, textVAO, textVBO, timeLeftStr, 10.0f, 570.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

        // Need to add ammo count when implemented
        std::string ammoCount = "Ammo: " + std::to_string(dataSys->carInfoList[0].ammoCount);
        RenderText(textShader, textVAO, textVBO, ammoCount, 10.0f, 10.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

        std::string score = "Score:";
        RenderText(textShader, textVAO, textVBO, score, 610.0f, 570.0f, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);

        for (int i = 0; i < dataSys->carInfoList.size(); i++) {
            float yOffset = i * 30;
            std::string playerScore = "Player " + std::to_string(i + 1) + ": " + std::to_string(dataSys->carInfoList[i].score);
            RenderText(textShader, textVAO, textVBO, playerScore, 610.0f, 540.0f - yOffset, 0.75f, glm::vec3(1.0f, 1.0f, 1.0f), Characters_gaegu);
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
            glm::vec3 cameraPosition = glm::vec3(0.0f, 50.0f, 0.0f);
            glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

            view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        }
        else {
            // Original view
            glm::vec3 offsetFromPlayer = glm::vec3(0.0f, 8.0f, 20.0f);
            camera.Position = playerPos + dataSys->getCamRotMat() * offsetFromPlayer; //we rotate camera with getCamRotMat
            glm::vec3 lookAtPoint = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(camera.Position, lookAtPoint, camera.Up);
        }

        shader.use();
        // car translating
        model = glm::translate(model, playerPos);
        // make it look forward
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = applyQuaternionToMatrix(model, playerRot);
        //model = glm::scale(model, glm::vec3(0.5f));

        // sending our matrixes to the shader
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, player1Texture);
        tank.Draw(shader);

        //funkyCube.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f));
        shader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture);
        plane.Draw(shader);

        //rendering all other entities starting at 2 (skipping player car and map)
        for (int i = 2; i < dataSys->entityList.size(); i++) {

            switch (dataSys->entityList[i].physType) {

            case (PhysicsType::CAR):

                //is the car alive? -> render it
                if (dataSys->GetCarInfoStructFromEntity(std::make_shared<Entity>(dataSys->entityList[i]))->isAlive) {
                    if (i == 1) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, player2Texture);
                    }
                    else if (i == 2) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, player3Texture);
                    }
                    else if (i == 3) {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, player4Texture);
                    }
                    else {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, player5Texture);
                    }

                    model = glm::mat4(1.0f);
                    model = glm::translate(model, dataSys->entityList[i].transform->getPos());
                    // make it look forward
                    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                    model = applyQuaternionToMatrix(model, dataSys->entityList[i].transform->getRot());
                    shader.setMat4("model", model);
                    tank.Draw(shader);
                }

                break;
            case (PhysicsType::PROJECTILE):

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, redTexture);

                model = glm::mat4(1.0f);
                model = glm::translate(model, dataSys->entityList[i].transform->getPos());
                shader.setMat4("model", model);
                projectile.Draw(shader);

                break;
            case (PhysicsType::POWERUP):

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, redTexture);

                model = glm::mat4(1.0f);
                model = glm::translate(model, dataSys->entityList[i].transform->getPos());
                shader.setMat4("model", model);
                powerup.Draw(shader);

                break;
            case (PhysicsType::STATIC):

                break;
            default:

                break;
            }
        }


    }

    // Setup UI if necessary
    if (dataSys->inMenu || dataSys->inResults) {
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
        else if(dataSys->inResults){
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
                else{
                    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultsP5, 0);
                }
            }
            
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // if not already bound
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // I think I need to either give it a projection
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove translation from the view matrix
    skyBoxShader.use();
    skyBoxShader.setMat4("projection", projection);
    skyBoxShader.setMat4("view", view);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    skyBoxShader.use();
    // render skybox
    glBindVertexArray(skyVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);


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