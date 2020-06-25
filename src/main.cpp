#include <VulkanManager.h>

using namespace std;

Skip::VulkanWindow* window;
Skip::VulkanManager* vulkanManager;
Skip::VulkanSwapchain* swapchain;
Skip::Camera* camera;

int main()
{
	// Main is our application
	// We will interact with Vulkan manager and vulkan window
	// GLFW will live in VulkanWindow and have public access

	// create components
	camera = new Skip::Camera(glm::vec3(2.0f, 2.0f, 2.0f));

	// create window
	// Window will create keys to events based on components
	window = new Skip::VulkanWindow(camera);
	window->init();
	vulkanManager = new Skip::VulkanManager(window);

	//setup validation layers
	#ifndef NODEBUG
		vulkanManager->_enableValidationLayers = true;
	#endif
	vulkanManager->_validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
	vulkanManager->init();
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

		Skip::UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//ubo.view = glm::lookAt(camera->GetUp(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = camera->GetViewMatrix();

		swapchain->updateUniformBuffer(ubo, currentImage);

		vulkanManager->drawFrame(currentImage);
	}
	vulkanManager->~VulkanManager();
	return 0;
}
