#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "RenderingSystem.h">
#include "Shader.h"
#include "Texture.h"
#include "Text.h"
#include "stb_image.h"

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

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int VAO, VBO;
unsigned int textVAO, textVBO;
std::map<char, Character> Characters_gaegu;
Shader textShader;
Shader shader;

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Commander P Scrapyard Challenge", NULL, NULL);
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
    
    // init VAO and VBO
    initVAO(vertices, sizeof(vertices), &VAO, &VBO);


    // depth for 3d rendering
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);  // or GL_FRONT, depending on your winding order


    // text shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textShader = Shader("src/vertex_shader_text.txt", "src/fragment_shader_text.txt");
    glm::mat4 textProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(textProjection));

    Characters_gaegu = initFont("src/assets/Gaegu-Bold.ttf");
    initTextVAO(&textVAO, &textVBO);

    // create textures
    texture1 = generateTexture("src/Textures/wood.jpg", true);
    stbi_set_flip_vertically_on_load(true); // to vertically flip the image
    texture2 = generateTexture("src/Textures/cat.jpg", true);

    // setting textures
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);
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

    // binding textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    shader.use();

    // camera setup stuff/ 3d transformations
    
    glm::mat4 model = glm::mat4(1.0f);
    // rotating transformation of cube based on time
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    // view matrix
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // this should be the camera matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // sending our matrixes to the shader
    unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(view));

    //int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(projection));

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);


    // 2d transformations
    //glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    //shader.use();
    // apply transformation
    //unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

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

    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //glBindVertexArray(0);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

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

GLFWwindow* RenderingSystem::getWindow() const{
    return window;
}
