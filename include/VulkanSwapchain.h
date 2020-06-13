#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <VulkanDevice.h>
#include <VulkanWindow.h>

namespace Skip {

	struct SwapchainDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapchain {

	public:
		VulkanSwapchain();
		VulkanSwapchain(VulkanDevice* vkDevice, VulkanWindow* vkWindow);
		~VulkanSwapchain();

		VkSwapchainKHR _swapChain;

		void rebuildSwapchain();

		SwapchainDetails querySwapchain(VulkanDevice* vkDevice, VulkanWindow* vkWindow);

		
	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	};


}