#pragma once

#include <vulkan/vulkan.h>

#include <map>

namespace Skip {

	struct GPUInfo;

	struct QueueFamilyIndices;
	
	class VulkanDevice {

	public:
		VulkanDevice();
		VulkanDevice(GPUInfo* gpu);
		~VulkanDevice();
		void init();

		GPUInfo* _gpuInfo;
		VkDevice _logicalDevice = VK_NULL_HANDLE;
	private:

	};


}