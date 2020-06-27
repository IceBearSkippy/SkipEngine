#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <vulkan/vulkan.h>
#include <string>

#include <Camera.h>

namespace Skip {

	class VulkanWindow {
		// Manages glfw and VkSurface
	public:
		VulkanWindow();
		VulkanWindow(Camera *camera);
		VulkanWindow(uint32_t width, uint32_t height, char* title, Camera *camera);
		~VulkanWindow();
		void init();
		bool shouldClose();
		void processKeys(float deltaTime);

		unsigned int _width;
		unsigned int _height;
		std::string _title;
		
		

		bool _framebufferResized = false;
		GLFWwindow* _glfw = nullptr;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		Camera* _camera = nullptr;
		
	private:
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
		static void MouseCallback(GLFWwindow* window, double xPos, double yPos);

	};


}