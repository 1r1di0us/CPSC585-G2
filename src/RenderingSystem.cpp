#include "RenderingSystem.h">

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);



// vertices for cubes
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};


std::map<char, Character> Characters_gaegu;

unsigned int texture1, texture2;

// notes for self, declare variable in header before using in cpp
// shader.use replaces glshader or whatever
// make sure to construct your variables

// constructor
RenderingSystem::RenderingSystem(){

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
    texture1 = generateTexture("src/Textures/wood.jpg", true);
    stbi_set_flip_vertically_on_load(true); // to vertically flip the image
    texture2 = generateTexture("src/Textures/cat.jpg", true);
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // init VAO and VBO
    initVAO(vertices, sizeof(vertices), &VAO, &VBO);

    // depth for 3d rendering
    glEnable(GL_DEPTH_TEST);

    // text shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Testing
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    //Transforms world coords to camera coords
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // Testing

    textShader = Shader("src/vertex_shader_text.txt", "src/fragment_shader_text.txt");
    glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    Characters_gaegu = initFont("src/assets/Gaegu-Bold.ttf");
    initTextVAO(&textVAO, &textVBO);
}


void RenderingSystem::updateRenderer() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // input
    processInput(window);

    // render
    // clear the colorbuffer and the depthbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rendering text
    RenderText(textShader, textVAO, textVBO, "hello!", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), Characters_gaegu);

    // activate shader
    shader.use();

    // camera setup stuff/ 3d transformations
    // this model is for a rotating one
    glm::mat4 model = glm::mat4(1.0f);
    //// rotating transformation of cube based on time
    //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    //shader.setMat4("model", model);

    // view matrix
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -7.0f));

    // this should be the camera matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Camera things
    view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
    model = glm::translate(model, glm::vec3(0.01f, 0.f, 0.f));
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.0f, 0.0f));

    // sending our matrixes to the shader
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);


    

    // binding textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    //render objects
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void RenderingSystem::processInput(GLFWwindow* window)
{
    GLdouble xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glm::vec3 current_pos;
    current_pos.x = (2.f / (float)800) * xPos - 1.f; // 800 = window width
    current_pos.y = (2.f / (float)600) * yPos - 1.f; // 600 = window height
    current_pos.y *= -1.f;



    float xoffset = (current_pos.x - lastX) * 1000.f;
    float yoffset = (lastY - current_pos.y) * 1000.f; // reversed since y-coordinates go from bottom to top
    lastX = current_pos.x;
    lastY = current_pos.y;
    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.Yaw += xoffset;
    camera.Pitch -= yoffset;  // REVERSE UP/DOWN DIRECTION 
    if (camera.Pitch > 89.0f)
        camera.Pitch = 89.0f;
    if (camera.Pitch < -89.0f)
        camera.Pitch = -89.0f;

    // Camera code (wasd)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(camera.FORWARD, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(camera.BACKWARD, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(camera.RIGHT, 0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(camera.LEFT, 0.01);
    }

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

GLFWwindow* RenderingSystem::getWindow() const{
    return window;
}