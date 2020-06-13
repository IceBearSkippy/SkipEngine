#include <VulkanDevice.h>

namespace Skip {

	VulkanDevice::VulkanDevice() { };
	VulkanDevice::VulkanDevice(GPUInfo* gpu) {
		_gpuInfo = gpu;
	};

	VulkanDevice::~VulkanDevice() {

	};


	QueueFamilyIndices QueueFamilyIndices::findQueueFamilies(GPUInfo* gpuInfo, VkSurfaceKHR& surface) {

		VkPhysicalDevice device = gpuInfo->device;
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}
		return indices;
	}

	bool QueueFamilyIndices::isComplete() {
		return graphicsFamily.has_value() &&
			presentFamily.has_value();
	}

}