#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <map>

namespace Skip {

	
	struct GPUInfo {
		VkPhysicalDevice device;
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;
		VkSampleCountFlagBits msaaSamples;
		int score;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete();
	};
	struct Queues {
		VkQueue _graphics = VK_NULL_HANDLE;
		VkQueue _present = VK_NULL_HANDLE;
		VkQueue _transfer = VK_NULL_HANDLE;
	};

	class VulkanDevice {

	public:
		VulkanDevice();
		VulkanDevice(GPUInfo* gpu);
		~VulkanDevice();
		void init();

		GPUInfo* _gpuInfo;
		Queues _queues;
		VkDevice _logicalDevice = VK_NULL_HANDLE;

		VkPhysicalDevice getPhysicalDevice() {
			return _gpuInfo->device;
		};

		VkDevice getLogicalDevice() {
			return _logicalDevice;
		};
	private:

	};


}