#include "RenderingSystem.h">

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void renderOBJ(const OBJModel& model);

std::map<char, Character> Characters_gaegu;

unsigned int player1Texture, player2Texture, player3Texture, player4Texture, player5Texture, redTexture, menuPlay, menuControls, menuQuit, controlsMenu, resultsP1, resultsP2, resultsP3, resultsP4, resultsP5, resultsTie, planeTexture, gunMetalTexture;

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

    // geom shader
    shader = Shader("src/vertex_shader.txt", "src/fragment_shader.txt");
    ourShader = Shader("src/model_loading_vertex.txt", "src/model_loading_fragment.txt");

    // create and set textures
    planeTexture = generateTexture("assets/Textures/wood.jpg", true);
    stbi_set_flip_vertically_on_load(true); // to vertically flip the image
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
    resultsP1 = generateTexture("assets/Textures/UI/resultsP1.jpg", true);
    resultsP2 = generateTexture("assets/Textures/UI/resultsP2.jpg", true);
    resultsP3 = generateTexture("assets/Textures/UI/resultsP3.jpg", true);
    resultsP4 = generateTexture("assets/Textures/UI/resultsP4.jpg", true);
    resultsP5 = generateTexture("assets/Textures/UI/resultsP5.jpg", true);
    resultsTie = generateTexture("assets/Textures/UI/resultsTie.jpg", true);

    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
    shader.setInt("texture3", 2);
    shader.setInt("texture4", 3);
    
    // depth for 3d rendering
    glEnable(GL_DEPTH_TEST);

    // text shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textShader = Shader("src/vertex_shader_text.txt", "src/fragment_shader_text.txt");
    glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    Characters_gaegu = initFont("./assets/Fonts/Candy Beans.otf");
    initTextVAO(&textVAO, &textVBO);

    this->tank = LoadModelFromPath("./assets/Models/tank.obj");
    this->ball = LoadModelFromPath("./assets/Models/ball.obj");
    this->plane = LoadModelFromPath("./assets/Models/finishedMap.obj");
    this->powerup = LoadModelFromPath("./assets/Models/building_E.obj");

    this->bedModel = LoadModelFromPath("./assets/Models/bed_double_A.obj");

    initOBJVAO(tank, &tankVAO, &tankVBO);
    initOBJVAO(ball, &ballVAO, &ballVBO);
    initOBJVAO(plane, &planeVAO, &planeVBO);
    initOBJVAO(powerup, &powerupVAO, &powerupVBO);
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

        glm::vec3 red = glm::vec3(1.0f, 0.5f, 0.5f);    // Adjusted to be darker
        glm::vec3 blue = glm::vec3(0.7f, 0.7f, 1.0f);
        glm::vec3 green = glm::vec3(0.7f, 1.0f, 0.7f);
        glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.7f);
        glm::vec3 pink = glm::vec3(1.0f, 0.75f, 0.75f);  // Adjusted to be darker
        for (int i = 0; i < dataSys->carInfoList.size(); i++) {
            glm::vec3 color;
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
            glm::vec3 offsetFromPlayer = glm::vec3(0.0f, 8.0f, 20.0f);
            camera.Position = playerPos + dataSys->getCamRotMat() * offsetFromPlayer; //we rotate camera with getCamRotMat
            glm::vec3 lookAtPoint = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);
            view = glm::lookAt(camera.Position, lookAtPoint, camera.Up);
        }

        // car translating
        model = glm::translate(model, playerPos);
        // make it look forward
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = applyQuaternionToMatrix(model, playerRot);

        // sending our matrixes to the shader
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // binding textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, player1Texture);

        //float angle = 45.0f;
        shader.setMat4("model", model);
        renderObject(tank, &tankVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        shader.setMat4("model", model);

        renderObject(plane, &planeVAO);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));

        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gunMetalTexture);
        shader.setMat4("model", model);
        renderObject(building, &buildingVAO);

        //rendering all other entities starting at the size of the static list + 1 (+1 isnt needed cause of index 0)
        for (int i = dataSys->STATIC_OBJECT_LIST.size(); i < dataSys->entityList.size(); i++) {

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
                    renderObject(tank, &tankVAO);
                }

                break;
            case (PhysicsType::PROJECTILE):

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, redTexture);

                model = glm::mat4(1.0f);
                model = glm::translate(model, dataSys->entityList[i].transform->getPos());
                shader.setMat4("model", model);
                renderObject(ball, &ballVAO);

                break;
            case (PhysicsType::POWERUP):

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, gunMetalTexture);

                model = glm::mat4(1.0f);
                model = glm::translate(model, dataSys->entityList[i].transform->getPos());
                shader.setMat4("model", model);
                renderObject(powerup, &powerupVAO);

                break;
            case (PhysicsType::STATIC):

                break;
            default:

                break;
            }
        }

        //ourShader.use();
        //bedModel.Draw(shader);

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



    // swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();

}

void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);

    // Calculate total size needed for vertex attributes
    size_t totalSize = model.vertices.size() * sizeof(glm::vec3) +
        model.textureCoordinates.size() * sizeof(glm::vec2) +
        model.normals.size() * sizeof(glm::vec3);

    // Allocate buffer memory
    glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

    // Copy vertex data
    glBufferSubData(GL_ARRAY_BUFFER, 0, model.vertices.size() * sizeof(glm::vec3), model.vertices.data());

    // Copy texture coordinate data
    glBufferSubData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3),
        model.textureCoordinates.size() * sizeof(glm::vec2), model.textureCoordinates.data());

    // Copy normal data
    glBufferSubData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3) +
        model.textureCoordinates.size() * sizeof(glm::vec2),
        model.normals.size() * sizeof(glm::vec3), model.normals.data());

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(model.vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    // Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(model.vertices.size() * sizeof(glm::vec3) +
        model.textureCoordinates.size() * sizeof(glm::vec2)));
    glEnableVertexAttribArray(2);
}


//void initOBJVAO(const OBJModel& model, unsigned int* VAO, unsigned int* VBO) {
//    glGenVertexArrays(1, VAO);
//    glGenBuffers(1, VBO);
//
//    glBindVertexArray(*VAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
//    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
//
//    // Vertex positions
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5, (void*)0);
//    glEnableVertexAttribArray(0);
//
//    // Texture coordinates
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(5), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//}


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
