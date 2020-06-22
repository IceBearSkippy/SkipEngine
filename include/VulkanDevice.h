#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <map>
#include <vector>

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
		static QueueFamilyIndices findQueueFamilies(GPUInfo* gpuInfo, VkSurfaceKHR& surface);
	};

	

	struct Queues {
		VkQueue graphics = VK_NULL_HANDLE;
		VkQueue present = VK_NULL_HANDLE;
		VkQueue transfer = VK_NULL_HANDLE;
	};

	class VulkanDevice {

	public:
		VulkanDevice();
		VulkanDevice(GPUInfo* gpu);
		~VulkanDevice();

		GPUInfo* _gpuInfo;
		Queues _queues;
		VkDevice _logicalDevice = VK_NULL_HANDLE;

		VkPhysicalDevice getPhysicalDevice() {
			return _gpuInfo->device;
		};

		VkDevice* getLogicalDevice() {
			return &_logicalDevice;
		};

	private:

	};


}