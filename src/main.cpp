#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "PhysicsSystem.h"
#include "Shader.h"

#include "PxPhysicsAPI.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include "CarSystem.h"
#include <chrono>
#include <thread>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//system creation and other important variables
std::vector<Entity> entityList;
PhysicsSystem physicsSys;
CarSystem carSys(physicsSys.getPhysics(), physicsSys.getScene(), physicsSys.getMaterial(), &entityList);
InputSystem inputSys;
RenderingSystem renderingSystem;
Camera camera;

//time related variables
const double TIMELIMIT = 180.0f;
const std::chrono::duration<double> PHYSICSUPDATESPEED = std::chrono::duration<double>(physicsSys.getTIMESTEP());
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point currentTime;
std::chrono::duration<double> totalTimePassed;
std::chrono::duration<double> totalTimeLeft;
std::chrono::high_resolution_clock::time_point previousIterationTime;
std::chrono::duration<double> physicsSimTime = PHYSICSUPDATESPEED;

int main() {
    
    //y axis rotation in radians
    int angle = PxPiDivFour;
    PxQuat carRotateQuat(angle, PxVec3(0.0f, 0.0f, 0.0f));

    //i have a list of cars (not entities) in the carsystem. can just pass that to physics system
    carSys.SpawnNewCar(PxVec3(0.0f, 0.0f, 0.0f), carRotateQuat);

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
    previousIterationTime = startTime;

    GLFWwindow* window;
    window = renderingSystem.getWindow();

    int FPSCOUNTER = 0;
    int seconds = 1;

    while (!glfwWindowShouldClose(window) && totalTimePassed.count() < TIMELIMIT) {

        //updating how much time has passed
        currentTime = std::chrono::high_resolution_clock::now();
        totalTimePassed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        totalTimeLeft = std::chrono::duration<double>(TIMELIMIT) - totalTimePassed;
        //printf("Time remaining: %f\n", TIMELIMIT - timePassed.count());

        //calculating the time passed since the last iteration of the loop
        physicsSimTime -= std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousIterationTime);
        previousIterationTime = currentTime;
        //printf("frame time: %f\n", physicsSimTime);

        totalTimePassed.count();

        FPSCOUNTER++;

        if (totalTimePassed.count() / seconds >= 1) {

            printf("FPS: %d\n", FPSCOUNTER);
            FPSCOUNTER = 0;
            seconds += 1;
        }

        // input
        // -----
        inputSys.checkIfGamepadsPresent(); //this is very crude, we are checking every frame how many controllers are connected.
        inputSys.getGamePadInput();
        inputSys.getKeyboardInput(window);
        if (inputSys.InputToMovement(carSys.GetVehicleFromRigidDynamic(entityList[0].collisionBox))) {
            carSys.Shoot(carSys.GetVehicleFromRigidDynamic(entityList[0].collisionBox));
        }

        //THIS IS BROKEN BELOW

        // render
        // ------
        renderingSystem.updateRenderer(entityList, camera, totalTimeLeft);

        //only updating the physics at max 60hz while everything else updates at max speed
        if (physicsSimTime.count() <= 0.0f) {
            physicsSys.stepPhysics(entityList, carSys.GetGVehicleList());
            physicsSimTime = PHYSICSUPDATESPEED;
        }

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