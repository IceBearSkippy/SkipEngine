#include <VulkanManager.h>

using namespace std;

Skip::VulkanWindow* window;
Skip::VulkanManager* vulkanManager;
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

	vulkanManager->init();
	while (!window->shouldClose()) {
		glfwPollEvents();
		vulkanManager->drawFrame();
	}

	vulkanManager->~VulkanManager();
	return 0;
}
