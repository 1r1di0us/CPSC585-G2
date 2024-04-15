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

//ZAIN THE GOAT!
std::vector<PxContactPairHeader> SharedDataSystem::contactPairs;

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
Camera camera;
AiSystem aiSys1;
AiSystem aiSys2;
AiSystem aiSys3;
AiSystem aiSys4;

//time related variables
const double TIMELIMIT = 50.0f;
const std::chrono::duration<double> PHYSICSUPDATESPEED = std::chrono::duration<double>(dataSys.TIMESTEP);
std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point currentTime;
std::chrono::high_resolution_clock::time_point lastTime;
std::chrono::duration<double> gameTimePassed;
std::chrono::duration<double> totalTimePassed;
std::chrono::duration<double> totalTimeLeft = std::chrono::duration<double>(TIMELIMIT);
std::chrono::high_resolution_clock::time_point previousIterationTime;
std::chrono::duration<double> timeUntilPhysicsUpdate = PHYSICSUPDATESPEED;
std::chrono::duration<double> deltaTime;
std::chrono::duration<double> durationZero = std::chrono::duration<double>::zero();
std::chrono::high_resolution_clock::time_point paused;
const double RESULTS_BUFFER = 2.0f;
std::chrono::duration<double> resultsLockLeft = std::chrono::duration<double>(RESULTS_BUFFER);

std::string menuMusic = "assets/Music/Cianwood City Remix.wav";
std::string gameMusic = "assets/Music/Miror B Remix.wav";
std::string resultsMusic = "assets/Music/Mario Strikers Results.wav";

int gameMusicChannelId;

int main() {

    //seeding the random number gen to be used throughout the game
    std::srand(static_cast<unsigned int>(PHYSICSUPDATESPEED.count()));

    //y axis rotation in radians
    float angle = PxTwoPi;
    PxQuat carRotateQuat(angle, PxVec3(0.0f, 1.0f, 0.0f));

    //fake constructor (real one didnt like me)
    dataSys.InitSharedDataSystem();

    soundSys.Init(&dataSys); //basically the constructor
    soundSys.LoadSound("assets/Music/PianoClusterThud.wav", false);
    soundSys.AddToSoundDict("Thud", "assets/Music/PianoClusterThud.wav");
    soundSys.LoadSound("assets/Music/PianoClusterBwud.wav", false);
    soundSys.AddToSoundDict("Bwud", "assets/Music/PianoClusterBwud.wav");
    soundSys.LoadSound("assets/Music/ParrySound.wav", false);
    soundSys.AddToSoundDict("Parry", "assets/Music/ParrySound.wav");
    soundSys.LoadSound("assets/Music/HeavenShort.wav", false);
    soundSys.AddToSoundDict("Heaven", "assets/Music/HeavenShort.wav");
    soundSys.LoadSound("assets/Music/ArmourDing.wav", false);
    soundSys.AddToSoundDict("Armour", "assets/Music/ArmourDing.wav");
    soundSys.LoadSound("assets/Music/PowerUp.wav", false);
    soundSys.AddToSoundDict("PowerUp", "assets/Music/PowerUp.wav");
    soundSys.LoadSound("assets/Music/Reload.wav", false);
    soundSys.AddToSoundDict("Reload", "assets/Music/Reload.wav");
    soundSys.LoadSound("assets/Music/CarCrash.wav", false);
    soundSys.AddToSoundDict("CarCrash", "assets/Music/CarCrash.wav");

    //initializing time variables
    startTime = std::chrono::high_resolution_clock::now();
    currentTime = startTime;
    lastTime = std::chrono::high_resolution_clock::now();
    previousIterationTime = lastTime;

    GLFWwindow* window;
    window = renderingSystem.getWindow();

    int FPSCOUNTER = 0;
    int seconds = 1;

    while (!glfwWindowShouldClose(window)) {

        //FPS counter var
        totalTimePassed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);

        // input
        // -----
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        inputSys.checkIfGamepadsPresent(); //this is very crude, we are checking every frame how many controllers are connected.
        inputSys.getGamePadInput();
        inputSys.getKeyboardInput(window);
        if (dataSys.inMenu) {
            inputSys.InputToMenu();
            lastTime = std::chrono::high_resolution_clock::now();
            totalTimeLeft = std::chrono::duration<double>(TIMELIMIT);
            resultsLockLeft = std::chrono::duration<double>(RESULTS_BUFFER);
            previousIterationTime = lastTime;
            
            if (dataSys.resultsMusicPlaying) {
                soundSys.UnLoadSound(resultsMusic);

                dataSys.resultsMusicPlaying = false;
            }

            if (dataSys.gameMusicPlaying) {
                soundSys.UnLoadSound(gameMusic);

                dataSys.gameMusicPlaying = false;
            }

            if (!dataSys.menuMusicPlaying) {
                soundSys.LoadSound(menuMusic, false, true);
                soundSys.PlaySound(menuMusic, FMOD_VECTOR{ 0, 0, 0 }, dataSys.MusicVolume);

                dataSys.menuMusicPlaying = true;
            }

            if (!dataSys.carsInitialized) {
                physicsSys.releaseActors();

                //i have a list of cars (not entities) in the carsystem. can just pass that to physics system
                carSys.SpawnNewCar(PxVec2(0.0f, 0.0f), carRotateQuat);

                //spawning more cars (need min 4 cars for respawning to work)
                carSys.SpawnNewCar(PxVec2(19.0f, 25.0f), carRotateQuat);
                carSys.SpawnNewCar(PxVec2(-19.0f, -19.0f), carRotateQuat);
                carSys.SpawnNewCar(PxVec2(-19.0f, 19.0f), carRotateQuat);
                carSys.SpawnNewCar(PxVec2(19.0f, -19.0f), carRotateQuat);
                aiSys1 = AiSystem(&dataSys, dataSys.GetVehicleFromRigidDynamic(dataSys.carInfoList[1].entity->collisionBox)); //call the constructors
                aiSys2 = AiSystem(&dataSys, dataSys.GetVehicleFromRigidDynamic(dataSys.carInfoList[2].entity->collisionBox));
                aiSys3 = AiSystem(&dataSys, dataSys.GetVehicleFromRigidDynamic(dataSys.carInfoList[3].entity->collisionBox));
                aiSys4 = AiSystem(&dataSys, dataSys.GetVehicleFromRigidDynamic(dataSys.carInfoList[4].entity->collisionBox));

                dataSys.carsInitialized = true;
            }
        }
        else if (dataSys.inResults) {
            if (dataSys.gameMusicPlaying) {
                soundSys.UnLoadSound(gameMusic);

                dataSys.gameMusicPlaying = false;
            }

            if (!dataSys.resultsMusicPlaying) {
                soundSys.LoadSound(resultsMusic, false, true);
                soundSys.PlaySound(resultsMusic, FMOD_VECTOR{ 0, 0, 0 }, dataSys.MusicVolume);

                dataSys.resultsMusicPlaying = true;
            }

            currentTime = std::chrono::high_resolution_clock::now();
            gameTimePassed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime);
            resultsLockLeft = resultsLockLeft - gameTimePassed;
            lastTime = currentTime;
            
            if (resultsLockLeft <= durationZero) {
                inputSys.InputToResults();
            }
        }
        else if (dataSys.inGameMenu) {
            inputSys.InputToGameMenu();
            if (!dataSys.gameMusicPaused) {
                soundSys.PauseChannel(gameMusicChannelId, true); // Pause the game music
                dataSys.gameMusicPaused = true;
            }
            lastTime = std::chrono::high_resolution_clock::now();
        }
        else {
            if (dataSys.menuMusicPlaying) {
                soundSys.UnLoadSound(menuMusic);
                dataSys.menuMusicPlaying = false;
            }

            if (!dataSys.gameMusicPlaying) {
                 soundSys.LoadSound(gameMusic, false, true);
                 gameMusicChannelId = soundSys.PlaySound(gameMusic, FMOD_VECTOR{ 0, 0, 0 }, dataSys.MusicVolume);

                 dataSys.gameMusicPlaying = true;
            }

            if (dataSys.gameMusicPaused) {
                soundSys.PauseChannel(gameMusicChannelId, false); // Pause the game music
                dataSys.gameMusicPaused = false;
            }
            //updating how much time has passed
            currentTime = std::chrono::high_resolution_clock::now();
            gameTimePassed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime);
            totalTimeLeft = totalTimeLeft - gameTimePassed;
            lastTime = currentTime;

            //calculating the total time passed since the last physics update
            deltaTime = currentTime - previousIterationTime;
            timeUntilPhysicsUpdate -= std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousIterationTime);
            previousIterationTime = currentTime;

            if (totalTimeLeft <= durationZero) {
                for (int i = 0; i < dataSys.carInfoList.size(); i++) {
                    if (dataSys.carInfoList[i].score > dataSys.carInfoList[dataSys.winningPlayer].score) {
                        dataSys.winningPlayer = i;
                    }
                }
                for (int i = 0; i < dataSys.carInfoList.size(); i++) {
                    if (dataSys.carInfoList[i].score == dataSys.carInfoList[dataSys.winningPlayer].score) {
                        if (dataSys.carInfoList[i].entity->name != dataSys.carInfoList[dataSys.winningPlayer].entity->name) {
                            dataSys.tieGame = true;
                        }
                    }
                }
                dataSys.inResults = true;
            }

            switch (inputSys.InputToMovement(deltaTime)) {
            //shoot
            case 1:
                if (carSys.Shoot(dataSys.carInfoList[0].entity->collisionBox)) {
                    dataSys.SoundsToPlay.push_back(std::make_pair(std::string("Thud"), PxVec3{ 0, 0, 0 }));
                }
                break;
            //parry
            case 2:
                if (dataSys.Parry(dataSys.carInfoList[0].entity->collisionBox)) {
                    //play audio cue + visual indicator
                }
                break;
            default:
                break;
            }

            if (aiSys1.update(deltaTime)) {
                if (carSys.Shoot(dataSys.carInfoList[1].entity->collisionBox)) {
                    PxVec3 soundOrigin = dataSys.getSoundRotMat() * (dataSys.carInfoList[1].entity->collisionBox->getGlobalPose().p - dataSys.carInfoList[0].entity->collisionBox->getGlobalPose().p);
                    dataSys.SoundsToPlay.push_back(std::make_pair(std::string("Thud"), soundOrigin));
                }
            }

            if (aiSys2.update(deltaTime)) {
                if (carSys.Shoot(dataSys.carInfoList[2].entity->collisionBox)) {
                    PxVec3 soundOrigin = dataSys.getSoundRotMat() * (dataSys.carInfoList[2].entity->collisionBox->getGlobalPose().p - dataSys.carInfoList[0].entity->collisionBox->getGlobalPose().p);
                    dataSys.SoundsToPlay.push_back(std::make_pair(std::string("Thud"), soundOrigin));
                }
            }

            if (aiSys3.update(deltaTime)) {
                if (carSys.Shoot(dataSys.carInfoList[3].entity->collisionBox)) {
                    PxVec3 soundOrigin = dataSys.getSoundRotMat() * (dataSys.carInfoList[3].entity->collisionBox->getGlobalPose().p - dataSys.carInfoList[0].entity->collisionBox->getGlobalPose().p);
                    dataSys.SoundsToPlay.push_back(std::make_pair(std::string("Thud"), soundOrigin));
                }
            }

            if (aiSys4.update(deltaTime)) {
                if (carSys.Shoot(dataSys.carInfoList[4].entity->collisionBox)) {
                    PxVec3 soundOrigin = dataSys.getSoundRotMat() * (dataSys.carInfoList[4].entity->collisionBox->getGlobalPose().p - dataSys.carInfoList[0].entity->collisionBox->getGlobalPose().p);
                    dataSys.SoundsToPlay.push_back(std::make_pair(std::string("Thud"), soundOrigin));
                }
            }

            //only updating the physics at max 60hz while everything else updates at max speed
            if (timeUntilPhysicsUpdate.count() <= 0.0f) {
                physicsSys.stepPhysics();
                timeUntilPhysicsUpdate = PHYSICSUPDATESPEED;
                carSys.RespawnAllCars();
                carSys.UpdateAllCarCooldowns();
                powerupSys.RespawnAllPowerups();
            }
            soundSys.PlayAllSounds();
        }

        // render
        // ------
        renderingSystem.updateRenderer(camera, totalTimeLeft, deltaTime);

        if (dataSys.quit) {
            break;
        }

        //if another second has passed, print the fps
        if (totalTimePassed.count() / seconds >= 1) {

            printf("FPS: %d\n", FPSCOUNTER);
            FPSCOUNTER = 0;
            seconds += 1;
        }

        //increases the frame counter
        FPSCOUNTER++;
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