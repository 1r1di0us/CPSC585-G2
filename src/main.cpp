#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "PhysicsSystem.h"
#include "shader_s.h"
#include "InputSystem.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//global vars (ideally temp, idk how that will work tho tbh)
PhysicsSystem physicsSys;
Entity playerCar;
InputSystem inputSys(&playerCar);
std::vector<Entity> entityList;

int main() {

    //y axis rotation in radians
    int angle = PxPiDivFour;
    PxQuat carRotateQuat(angle, PxVec3(0.0f, 1.0f, 0.0f));

    //creating the player car entity
    playerCar.name = "playerCar";
    playerCar.physType = PhysicsType::CAR;
    playerCar.car = new Car(playerCar.name.c_str(), PxVec3(0.0f, 0.0f, 0.0f), carRotateQuat, physicsSys.getPhysics(), physicsSys.getScene(), physicsSys.getGravity(), physicsSys.getMaterial());

    //adds the car to the carlist and the entity list
    //TODO: add car to rigid dynamic list
    physicsSys.carList.emplace_back(playerCar.car);
    entityList.emplace_back(playerCar);

    ////creating the second car entity
    //Entity car2;
    //car2.name = "car2";
    //car2.physType = PhysicsType::CAR;
    //car2.car = new Car(playerCar.name.c_str(), PxVec3(10.0f, 0.0f, -10.0f), PxQuat(PxIdentity), physicsSys.getPhysics(), physicsSys.getScene(), physicsSys.getGravity(), physicsSys.getMaterial());

    //adding the second car to the entity list
    //physicsSys.carList.emplace_back(car2.car);
    //entityList.emplace_back(car2);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
// ------------------------------------
    Shader ourShader("./src/shader.vs", "./src/shader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        inputSys.checkIfGamepadsPresent(); //this is very crude, we are checking every frame how many controllers are connected.

        // input
        // -----
        //processInput(window);
        inputSys.getGamePadInput();
        inputSys.getKeyboardInput(window);
        std::cout << std::endl;
            
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        //physicsSys.gScene->simulate(1.0f / 60.0f);
        //physicsSys.gScene->fetchResults(true);
        //physicsSys.updateTransforms();

        //physx::PxVec3 objPos = physicsSys.getPos(50);
        //std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
        //std::cout << entityList[50].transform->pos.y << std::endl;
        physicsSys.stepPhysics(entityList);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    physicsSys.cleanPhysicsSystem();
    return 0;
}

void shoot(Entity* car) {

    Entity projectile;

    entityList.emplace_back(projectile);

    //get the car pos and touch it up a bit to get spawn pos
    projectile.name = "bullet";
    projectile.physType = PhysicsType::PROJECTILE;
    physicsSys.shootProjectile(car, &projectile);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //will shoot a projectile
    //FIXME: broken af rn. needs IO to be working to properly test
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        shoot(&playerCar);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/*#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ftlib;

error = FT_Init_FreeType( &ftlib );
if (error)
{
    ... an error occurred during library initialization ...
}*/
