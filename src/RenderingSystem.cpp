#include "RenderingSystem.h">

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void renderOBJ(const OBJModel& model);

std::map<char, Character> Characters_gaegu;

unsigned int blueTexture, catTexture, redTexture;

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
RenderingSystem::RenderingSystem() {

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

    // geom shader
    shader = Shader("src/vertex_shader.txt", "src/fragment_shader.txt");

    // create and set textures
    blueTexture = generateTexture("src/Textures/cat.jpg", true);
    stbi_set_flip_vertically_on_load(true); // to vertically flip the image
    catTexture = generateTexture("src/Textures/cat.jpg", true);
    redTexture = generateTexture("src/Textures/red.jpg", true);
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
    shader.setInt("texture3", 2);

    // depth for 3d rendering
    glEnable(GL_DEPTH_TEST);

    // text shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textShader = Shader("src/vertex_shader_text.txt", "src/fragment_shader_text.txt");
    glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    Characters_gaegu = initFont("./assets/Gaegu-Bold.ttf");
    initTextVAO(&textVAO, &textVBO);

    this->tank = LoadModelFromPath("./assets/Models/tank.obj");
    this->building = LoadModelFromPath("./assets/Models/building_E.obj");
    this->ball = LoadModelFromPath("./assets/Models/ball.obj");
    this->plane = LoadModelFromPath("./assets/Models/planeHugeWithWalls.obj");

    initOBJVAO(tank, &tankVAO, &tankVBO);
    initOBJVAO(building, &buildingVAO, &buildingVBO);
    initOBJVAO(ball, &ballVAO, &ballVBO);
    initOBJVAO(plane, &planeVAO, &planeVBO);
}


void RenderingSystem::updateRenderer(std::shared_ptr<std::vector<Entity>> entityList, Camera camera, std::chrono::duration<double> timeLeft) {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // input
    processInput(window);

    // render
    // clear the colorbuffer and the depthbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rendering text
    // Convert timeLeft to seconds
    int timeLeftInSeconds = static_cast<int>(timeLeft.count());

    // Convert timeLeftInSeconds to string
    std::string timeLeftStr = "Time Left: " + std::to_string(timeLeftInSeconds);
    RenderText(textShader, textVAO, textVBO, timeLeftStr, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), Characters_gaegu);

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
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // getting the car position and rotation
    glm::vec3 playerPos = entityList->at(0).transform->getPos();
    glm::quat playerRot = entityList->at(0).transform->getRot();
    //std::cout << playerPos.x << ":" << playerPos.y << ":" << playerPos.z << std::endl;

    // Calculate the point the camera should look at (e.g., slightly above the player)
    glm::vec3 offsetFromPlayer = glm::vec3(0.0f, 8.0f, 20.0f);
    camera.Position = playerPos + offsetFromPlayer;
    glm::vec3 lookAtPoint = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);

    //// Camera things
    view = glm::lookAt(camera.Position, lookAtPoint, camera.Up);

    //bird's eye view
    //Define camera parameters
    //glm::vec3 cameraPosition = glm::vec3(0.0f, 50.0f, 0.0f); // Position the camera above the scene
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Look at the center of the scene
    //glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f); // Define the up vector

    //// Calculate the view matrix using glm::lookAt
    //view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

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
    glBindTexture(GL_TEXTURE_2D, catTexture);

    //float angle = 45.0f;
    shader.setMat4("model", model);
    renderObject(tank, &tankVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blueTexture);

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 0.0f, 5.0f));
    shader.setMat4("model", model);

    renderObject(plane, &planeVAO);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, redTexture);
    shader.setMat4("model", model);
    renderObject(building, &buildingVAO);

    //rendering all other entities starting at 1 (skipping player car)
    for (int i = 1; i < entityList->size(); i++) {

        switch (entityList->at(i).physType) {

        case (PhysicsType::CAR):

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, catTexture);

            model = glm::mat4(1.0f);
            model = glm::translate(model, entityList->at(i).transform->getPos());
            // make it look forward
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
            model = applyQuaternionToMatrix(model, entityList->at(i).transform->getRot());
            shader.setMat4("model", model);
            renderObject(tank, &tankVAO);

            break;
        case (PhysicsType::PROJECTILE):

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, redTexture);

            model = glm::mat4(1.0f);
            model = glm::translate(model, entityList->at(i).transform->getPos());
            shader.setMat4("model", model);
            renderObject(ball, &ballVAO);

            break;
        case (PhysicsType::STATIC):

            break;
        default:

            break;
        }
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
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(glm::vec3), &model.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //Texture vertex attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
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
