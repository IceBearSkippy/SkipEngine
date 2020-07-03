#include <VulkanManager.h>
#include <objects/SkipObject.h>
#include <objects/Model.h>
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
		"resources/models/viking_room.obj"
	);
	skipObjects.push_back(modelObject);
	Skip::Model* test = new Skip::Model(
		glm::vec3(2.0f, 1.0f, 0.0f)
	);
	skipObjects.push_back(test);

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
		test->_ubo.model = test->GetPositionMatrix() * Skip::buildScale(0.2f, 0.2f, 0.4f);
		test->_ubo.view = camera->GetViewMatrix();
		test->_ubo.proj = glm::perspective(glm::radians(45.0f), swapchain->_swapChainExtent.width / (float)swapchain->_swapChainExtent.height, 0.1f, 10.0f);
		test->_ubo.proj[1][1] *= -1;
		
		swapchain->updateUniformBuffers(currentImage);

		vulkanManager->drawFrame(currentImage);
	}
	vulkanManager->~VulkanManager();
	return 0;
}
