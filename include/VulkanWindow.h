#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>

namespace Skip {

	class VulkanWindow {
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
		VkSurfaceKHR _surface;
	private:
		


	};


}