#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "PhysicsSystem.h"
#include "Shader.h"

#include "PxPhysicsAPI.h"
#include "RenderingSystem.h"
#include "InputSystem.h"
#include "SoundSystem.h"
#include "CarSystem.h"
#include "AiSystem.h"
#include "SharedDataSystem.h"
#include "PowerupSystem.h"
#include <chrono>
#include <thread>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//system creation and other important variables
SharedDataSystem dataSys;
PhysicsSystem physicsSys(&dataSys);
CarSystem carSys(&dataSys);
PowerupSystem powerupSys(&dataSys);
InputSystem inputSys(&dataSys);
RenderingSystem renderingSystem(&dataSys);
SoundSystem soundSys;
AiSystem aiSys(&dataSys);
Camera camera;

//time related variables
const double TIMELIMIT = 180.0f;
const std::chrono::duration<double> PHYSICSUPDATESPEED = std::chrono::duration<double>(dataSys.TIMESTEP);
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point currentTime;
std::chrono::duration<double> totalTimePassed;
std::chrono::duration<double> totalTimeLeft;
std::chrono::high_resolution_clock::time_point previousIterationTime;
std::chrono::duration<double> timeUntilPhysicsUpdate = PHYSICSUPDATESPEED;
std::chrono::duration<double> deltaTime;

int main() {
    
    //y axis rotation in radians
    int angle = PxPiDivFour;
    PxQuat carRotateQuat(angle, PxVec3(0.0f, 0.0f, 0.0f));

    //spawning player car
    carSys.SpawnNewCar(PxVec3(0.0f, 0.0f, 0.0f), carRotateQuat);

    //spawning more cars (need min 4 cars for respawning to work)
    carSys.SpawnNewCar(PxVec3(19.0f, 0.0f, 19.0f), carRotateQuat);
    carSys.SpawnNewCar(PxVec3(-19.0f, 0.0f, -19.0f), carRotateQuat);
    carSys.SpawnNewCar(PxVec3(-19.0f, 0.0f, 19.0f), carRotateQuat);
    carSys.SpawnNewCar(PxVec3(19.0f, 0.0f, -19.0f), carRotateQuat);

    //spawning powerups
    powerupSys.SpawnPowerup(PxVec3(0.0f, 0.0f, 5.0f), PowerupType::AMMO);

    soundSys.Init();
    soundSys.LoadSound("assets/PianoClusterThud.wav", false);

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

        //calculating the total time passed since the last physics update
        deltaTime = currentTime - previousIterationTime;
        timeUntilPhysicsUpdate -= std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousIterationTime);
        previousIterationTime = currentTime;

        //increases the frame counter
        FPSCOUNTER++;

        //if another second has passed, print the fps
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
        if (dataSys.inMenu) {
            inputSys.InputToMenu();
        }
        else {
            if (inputSys.InputToMovement(deltaTime)) {
                carSys.Shoot(std::make_shared<Entity>(dataSys.entityList[0])->collisionBox);
                soundSys.PlaySound("assets/PianoClusterThud.wav");
            }

            if (aiSys.update(dataSys.GetVehicleFromRigidDynamic(dataSys.entityList[1].collisionBox), deltaTime)) {
                carSys.Shoot(std::make_shared<Entity>(dataSys.entityList[1])->collisionBox);
                soundSys.PlaySound("assets/PianoClusterThud.wav");
            }
        }

        // render
        // ------
        renderingSystem.updateRenderer(std::make_shared<std::vector<Entity>>(dataSys.entityList), camera, totalTimeLeft);

        //only updating the physics at max 60hz while everything else updates at max speed
        if (timeUntilPhysicsUpdate.count() <= 0.0f) {
            physicsSys.stepPhysics();
            timeUntilPhysicsUpdate = PHYSICSUPDATESPEED;
            carSys.RespawnAllCars();
            powerupSys.RespawnAllPowerups();
        }

        if (dataSys.quit) {
            break;
        }

    }

    //game loop ends
    printf("\nGAME LOOP ENDED\n");
    soundSys.Shutdown();

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