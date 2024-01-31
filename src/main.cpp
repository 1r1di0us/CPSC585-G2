#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "PhysicsSystem.h"
#include "Shader.h"

#include "PxPhysicsAPI.h"
#include "RenderingSystem.h"


int main()
{
    PhysicsSystem physicsSys;

    // Simulate at 60fps
    // std::cout << physicsSys.rigidDynamicList.size() << std::endl;

    std::vector<Entity> entityList;
    entityList.reserve(465);

    for (int i = 0; i < 465; i++) {
        entityList.emplace_back();
        entityList.back().name = "box";
        entityList.back().transform = physicsSys.transformList[i];
        entityList.back().model = NULL;
    }


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    RenderingSystem renderingSystem;
    if (renderingSystem.getWindow() == nullptr) {
        std::cerr << "Failed to create GLFW window in RenderingSystem" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(renderingSystem.getWindow())) {
        renderingSystem.render();

        // You can add other game loop logic here
        physicsSys.gScene->simulate(1.0f / 60.0f);
        physicsSys.gScene->fetchResults(true);
        physicsSys.updateTransforms();

        physx::PxVec3 objPos = physicsSys.getPos(50);
        std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
        std::cout << entityList[50].transform->pos.y << std::endl;

        glfwSwapBuffers(renderingSystem.getWindow());
        glfwPollEvents();
    }
}
    // need to give the window somehow
    //while(!glfwWindowShouldClose(window)){

    //    physicsSys.gScene->simulate(1.0f / 60.0f);
    //    physicsSys.gScene->fetchResults(true);
    //    physicsSys.updateTransforms();

    //    physx::PxVec3 objPos = physicsSys.getPos(50);
    //    std::cout << "x: " << objPos.x << " y: " << objPos.y << " z: " << objPos.z << std::endl;
    //    std::cout << entityList[50].transform->pos.y << std::endl;
    //}


