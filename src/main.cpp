#include <VulkanManager.h>
#include <Camera.h>
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

	// create window
	window = new Skip::VulkanWindow();
	window->init();
	vulkanManager = new Skip::VulkanManager(window);

	//setup validation layers
	#ifndef NODEBUG
		vulkanManager->_enableValidationLayers = true;
	#endif
	vulkanManager->_validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	camera = new Skip::Camera(glm::vec3(2.0f, 2.0f, 2.0f));
	vulkanManager->init();
	swapchain = vulkanManager->_vulkanSwapchain;

	//TODO: enable mouse events for created window
	while (!window->shouldClose()) {
		glfwPollEvents();

		uint32_t currentImage = swapchain->stageFrame();
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		Skip::UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), -time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		swapchain->updateUniformBuffer(ubo, currentImage);

		vulkanManager->drawFrame(currentImage);
	}
	vulkanManager->~VulkanManager();
	return 0;
}
