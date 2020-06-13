#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// allows us to avoid using alignas
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <fstream>

#include <VulkanDevice.h>
#include <VulkanWindow.h>

namespace Skip {

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

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

		VulkanDevice* _vkDevice = nullptr;
		VulkanWindow* _vkWindow = nullptr;

		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		std::vector<VkImageView> _swapChainImageViews;
		VkRenderPass _renderPass;
		VkDescriptorSetLayout _descriptorSetLayout;

		SwapchainDetails querySwapchain();

		void recreateSwapChain();
		void cleanupSwapChain();
	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		void createSwapChain();
		void createImageViews();
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
			uint32_t mipLevels);

		void createRenderPass();
		VkFormat findDepthFormat();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, 
			VkFormatFeatureFlags features);

		void createDescriptorSetLayout();

		void createGraphicsPipeline();
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};


}