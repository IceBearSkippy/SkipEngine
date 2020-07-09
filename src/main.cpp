#include <VulkanManager.h>
#include <objects/SkipObject.h>
#include <objects/Model.h>
#include <objects/Cube.h>
#include <objects/Sphere.h>
using namespace std;

Skip::VulkanWindow* window;
Skip::VulkanManager* vulkanManager;
Skip::VulkanSwapchain* swapchain;
Skip::Camera* camera;

int main()
{
    bool enableValidationLayers = false;
    #ifndef NODEBUG
        enableValidationLayers = true;
    #endif

    // Main is our application
    // We will interact with Vulkan manager and vulkan window
    // GLFW will live in VulkanWindow and have public access

    // create components
    camera = new Skip::Camera(glm::vec3(2.0f, 2.0f, 2.0f));

    std::vector<Skip::SkipObject*> skipObjects;

    Skip::Model* modelObject = new Skip::Model(
        glm::vec3(0.0f, 0.0f, 0.0f),
        "resources/textures/viking_room.png",
        "resources/models/viking_room.obj",
        true
    );
    skipObjects.push_back(modelObject);
    Skip::Cube* cube = new Skip::Cube(
        glm::vec3(2.0f, 1.0f, 0.0f), DEFAULT_TEXTURE, false
    );
    skipObjects.push_back(cube);

    Skip::Sphere* sphere = new Skip::Sphere(
        glm::vec3(-2.0f, 1.0f, 0.0f), 12, DEFAULT_TEXTURE, false
    );
    skipObjects.push_back(sphere);

    // create window
    // Window will create keys to events based on components
    window = new Skip::VulkanWindow(camera);
    window->init();

    vulkanManager = new Skip::VulkanManager(window, skipObjects, enableValidationLayers);
    swapchain = vulkanManager->_vulkanSwapchain;

    uint32_t currentImage;
    float currentTime, deltaTime;
    float lastTime = 0.0;
    while (!window->shouldClose()) {
        glfwPollEvents();

        currentImage = swapchain->stageFrame();
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window->processKeys(deltaTime);

        modelObject->_ubo.model = Skip::buildRotateX(glm::radians(90.0f));
        modelObject->_ubo.view = camera->GetViewMatrix();
        modelObject->_ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        modelObject->_ubo.proj[1][1] *= -1;
        cube->_ubo.model = cube->GetPositionMatrix() * Skip::buildScale(0.2f, 0.2f, 0.2f);
        cube->_ubo.view = camera->GetViewMatrix();
        cube->_ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        cube->_ubo.proj[1][1] *= -1;

        sphere->_ubo.model = sphere->GetPositionMatrix() * Skip::buildScale(0.2f, 0.2f, 0.2f);
        sphere->_ubo.view = camera->GetViewMatrix();
        sphere->_ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
        sphere->_ubo.proj[1][1] *= -1;
        
        swapchain->updateUniformBuffers(currentImage);

        vulkanManager->drawFrame(currentImage);
    }
    vulkanManager->~VulkanManager();
    return 0;
}
