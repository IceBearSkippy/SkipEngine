#pragma once

#include <map>

#include <VulkanManager.h>
namespace Skip {

	struct GPUInfo;

	class VulkanDevice {
		// Manages glfw and VkSurface
	public:
		VulkanDevice();
		VulkanDevice(const VulkanManager* vkManager, GPUInfo* gpu);
		~VulkanDevice();
		void init();

		const VulkanManager* _vkManager;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _logicalDevice = VK_NULL_HANDLE;
	private:

	};


}