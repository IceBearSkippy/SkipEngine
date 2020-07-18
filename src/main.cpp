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
    scene->addObject(modelObject);
    Skip::Cube* cube = new Skip::Cube(
        glm::vec3(2.0f, 1.0f, 0.0f), DEFAULT_TEXTURE, false
    );
    scene->addObject(cube);

    Skip::Sphere* sphere = new Skip::Sphere(
        glm::vec3(2.0f, 1.5f, 0.0f), 12, DEFAULT_TEXTURE, false
    );
    scene->addObject(sphere);

    // create window
    // Window will create keys to events based on components
    window = new Skip::VulkanWindow(scene);
    window->init();

    vulkanManager = new Skip::VulkanManager(window, scene, enableValidationLayers);
    swapchain = vulkanManager->_vulkanSwapchain;

    uint32_t currentImage;
    float currentTime, deltaTime;
    float lastTime = 0.0;
    glm::mat4 mvpMat;
    while (!window->shouldClose()) {
        glfwPollEvents();

        currentImage = swapchain->stageFrame();
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window->processKeys(deltaTime);

        modelObject->_mvpUBO.model = Skip::buildRotateX(glm::radians(90.0f));
        modelObject->_mvpUBO.view = scene->_camera->GetViewMatrix();
        modelObject->_mvpUBO.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        modelObject->_mvpUBO.proj[1][1] *= -1;

        cube->_mvpUBO.model = cube->GetPositionMatrix() * Skip::buildScale(0.2f, 0.2f, 0.2f);
        cube->_mvpUBO.view = scene->_camera->GetViewMatrix();
        cube->_mvpUBO.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        cube->_mvpUBO.proj[1][1] *= -1;
        mvpMat = cube->_mvpUBO.proj * cube->_mvpUBO.view * cube->_mvpUBO.model;
        cube->_mvpUBO.norm = glm::transpose(glm::inverse((mvpMat)));

        sphere->_mvpUBO.model = sphere->GetPositionMatrix() * Skip::buildScale(0.2f, 0.2f, 0.2f);
        sphere->_mvpUBO.view = scene->_camera->GetViewMatrix();
        sphere->_mvpUBO.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        sphere->_mvpUBO.proj[1][1] *= -1;
        sphere->_lightUBO.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 100.0f;
        sphere->_lightUBO.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 10.0f;
        sphere->_lightUBO.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) * 10.0f;

        mvpMat = sphere->_mvpUBO.proj * sphere->_mvpUBO.view * sphere->_mvpUBO.model;
        sphere->_mvpUBO.norm = glm::transpose(glm::inverse((mvpMat)));
        
        swapchain->updateUniformBuffers(currentImage);

        vulkanManager->drawFrame(currentImage);
    }
    vulkanManager->~VulkanManager();
    return 0;
}
