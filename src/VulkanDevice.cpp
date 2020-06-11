#include <VulkanDevice.h>

namespace Skip {

	VulkanDevice::VulkanDevice() { };
	VulkanDevice::VulkanDevice(GPUInfo* gpu) {
		_gpuInfo = gpu;
	};

	VulkanDevice::~VulkanDevice() {

	};

	void VulkanDevice::init() {
	};

}