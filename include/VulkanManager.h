#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <optional>
#include <vector>
#include <iostream>


const std::string ENGINE_NAME = "SkipEngine";
const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 2, 1);

const uint32_t ENGINE_API_VERSION = VK_API_VERSION_1_2;

namespace Skip {

	class VulkanWindow;

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete();
	};

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface);

	class VulkanManager {
	public:
		VulkanManager(VulkanWindow* window);
		~VulkanManager();

		void init();
		bool checkValidationLayerSupport();

		VulkanWindow* _window;
		bool _enableValidationLayers;
		std::vector<const char*> _validationLayers;
		std::vector<const char*> getRequiredExtensions();
		

	private:
		void createInstance();
		void createSurface();


		VkDebugUtilsMessengerEXT _debugMessenger;
		void setupDebugMessenger();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkInstance _instance = VK_NULL_HANDLE;
		VkResult createDebugUtilsMessengerEXT(VkInstance instance, const
			VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
			VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
			pDebugMessenger);
		void destroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger, const
			VkAllocationCallbacks* pAllocator);
	};


}
