#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "PhysicsSystem.h"
#include "Shader.h"

#include "PxPhysicsAPI.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//global vars (ideally temp, idk how that will work tho tbh)
PhysicsSystem physicsSys;
Entity playerCar;
InputSystem inputSys;
std::vector<Entity> entityList;
RenderingSystem renderingSystem;

//time related variables
const double TIMELIMIT = 180.0f;
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point currentTime;
std::chrono::duration<double> timePassed;

int main() {
    
    //y axis rotation in radians
    int angle = PxPiDivFour;
    PxQuat carRotateQuat(angle, PxVec3(0.0f, 1.0f, 0.0f));

    //creating the player car entity
    playerCar.name = "playerCar";
    playerCar.physType = PhysicsType::CAR;
    playerCar.transform = new Transform();
    playerCar.car = new Car(playerCar.name.c_str(), PxVec3(0.0f, 0.0f, 0.0f), carRotateQuat, physicsSys.getPhysics(), physicsSys.getScene(), physicsSys.getGravity(), physicsSys.getMaterial());

    //adds the car to the all important lists
    physicsSys.carList.emplace_back(playerCar.car);
    entityList.emplace_back(playerCar);

    ////creating the second car entity
    //Entity car2;
    //car2.name = "car2";
    //car2.physType = PhysicsType::CAR;
    //car2.transform = new Transform();
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

    //setting the round timer (will be moved to appropriate place when it is created)
    startTime = std::chrono::high_resolution_clock::now();

    GLFWwindow* window;
    window = renderingSystem.getWindow();

    while (!glfwWindowShouldClose(window) && timePassed.count() < TIMELIMIT) {
        
        // input
        // -----
        inputSys.checkIfGamepadsPresent(); //this is very crude, we are checking every frame how many controllers are connected.
        inputSys.getGamePadInput();
        inputSys.getKeyboardInput(window);
        inputSys.InputToMovement(&playerCar);
        
        // render
        // ------
      
        renderingSystem.updateRenderer(entityList);

        physicsSys.stepPhysics(entityList);

        //updating how much time has passed
        currentTime = std::chrono::high_resolution_clock::now();
        timePassed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        printf("Time remaining: %f\n", TIMELIMIT - timePassed.count());
    }

    //game loop ends
    printf("\nGAME LOOP ENDED\n");

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    physicsSys.cleanPhysicsSystem();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}