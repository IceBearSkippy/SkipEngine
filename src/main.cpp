#include <VulkanManager.h>
#include <objects/SkipObject.h>
#include <objects/Model.h>
#include <objects/Cube.h>
#include <objects/Sphere.h>
using namespace std;

Skip::VulkanWindow* window;
Skip::VulkanManager* vulkanManager;
Skip::VulkanSwapchain* swapchain;
Skip::SkipScene* scene;

int main()
{
    bool enableValidationLayers = false;
    #ifndef NODEBUG
        enableValidationLayers = true;
    #endif

    // Main is our application
    // We will interact with Vulkan manager and vulkan window
    // GLFW will live in VulkanWindow and have public access

    // create scene with camera position
    scene = new Skip::SkipScene(glm::vec3(2.0f, 2.0f, 2.0f));

    std::vector<Skip::SkipObject*> skipObjects;

    Skip::Model* modelObject = new Skip::Model(
        glm::vec3(0.0f, 0.0f, 0.0f),
        "resources/textures/viking_room.png",
        "resources/models/viking_room.obj",
        true
    );
    Skip::Sphere* lightSphere = new Skip::Sphere(
        "LightSphere", glm::vec3(1.0f, 1.5f, 0.0f), 12, DEFAULT_TEXTURE, false
    );
    Skip::Sphere* sphere = new Skip::Sphere(
        glm::vec3(1.0f, 0.2f, 0.0f), 24, DEFAULT_TEXTURE, false
    );


    lightSphere->_mvpUBO.model = lightSphere->GetPositionMatrix() * Skip::buildScale(0.1f, 0.1f, 0.1f);
    lightSphere->_lightUBO.globalAmbient *= 3.0f;
    lightSphere->_lightUBO.ambient *= 100.0f;
    lightSphere->_lightUBO.diffuse *= 50.0f;
    lightSphere->_lightUBO.specular *= 0.5f;

    sphere->_mvpUBO.model = sphere->GetPositionMatrix() * Skip::buildScale(0.1f, 0.1f, 0.1f);

    modelObject->_mvpUBO.model = Skip::buildRotateX(glm::radians(90.0f));

    scene->addObject(lightSphere);
    scene->addObject(modelObject, lightSphere);
    scene->addObject(sphere, lightSphere);

    // create window
    // Window will create keys to events based on components
    window = new Skip::VulkanWindow(scene);
    window->init();

    vulkanManager = new Skip::VulkanManager(window, scene, enableValidationLayers);
    swapchain = vulkanManager->_vulkanSwapchain;

    uint32_t currentImage;
    float currentTime, deltaTime;
    float lastTime = 0.0;
    glm::mat4 mvMat;

    while (!window->shouldClose()) {
        glfwPollEvents();

        currentImage = swapchain->stageFrame();
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window->processKeys(deltaTime);

        modelObject->_mvpUBO.view = scene->_camera->GetViewMatrix();
        mvMat = modelObject->_mvpUBO.view * modelObject->_mvpUBO.model;
        modelObject->_mvpUBO.norm = glm::transpose(glm::inverse((mvMat)));

        lightSphere->_mvpUBO.view = scene->_camera->GetViewMatrix();
        mvMat = lightSphere->_mvpUBO.view * lightSphere->_mvpUBO.model;
        lightSphere->_mvpUBO.norm = glm::transpose(glm::inverse((mvMat)));

        sphere->_mvpUBO.view = scene->_camera->GetViewMatrix();
        mvMat = sphere->_mvpUBO.view * sphere->_mvpUBO.model;
        sphere->_mvpUBO.norm = glm::transpose(glm::inverse((mvMat)));

        swapchain->updateUniformBuffers(currentImage);

        vulkanManager->drawFrame(currentImage);
    }
    vulkanManager->~VulkanManager();
    return 0;
}
