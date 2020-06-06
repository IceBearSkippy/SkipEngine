#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <VulkanManager.h>

namespace Skip {

	class VulkanWindow {
		// Manages glfw and VkSurface
	public:
		VulkanWindow();
		VulkanWindow(uint32_t width, uint32_t height, char* title);
		~VulkanWindow();
		void init();
		bool shouldClose();

		unsigned int _width;
		unsigned int _height;
		std::string _title;

		bool _framebufferResized = false;
		GLFWwindow* _glfw;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
	private:
		


	};


}