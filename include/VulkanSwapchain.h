#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// allows us to avoid using alignas
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <stb/stb_image.h>

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
	
	//UBO
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	// this will only support simple uv mapping
	struct ModelObject {
		std::string texturePath;
		std::string modelPath;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		uint32_t mipLevels;

		std::vector<Vertex> vertices;
		std::unordered_map<Vertex, uint32_t> uniqueVertices;
		std::vector<uint32_t> indices;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
	};

	// hash function for Vertex
	struct SwapchainDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapchain {

	public:
		VulkanSwapchain();
		VulkanSwapchain(VulkanDevice* vkDevice, VulkanWindow* vkWindow, std::vector<ModelObject> modelObjects);
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
		VkPipelineLayout _pipelineLayout;
		VkPipeline _graphicsPipeline;
		VkCommandPool _commandPool;
		VkImage _colorImage;
		VkDeviceMemory _colorImageMemory;
		VkImageView _colorImageView;
		VkImage _depthImage;
		VkDeviceMemory _depthImageMemory;
		VkImageView _depthImageView;
		std::vector<VkFramebuffer> _swapChainFramebuffers;
		std::vector<ModelObject>  _modelObjects;
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

		void createCommandPool();
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void createColorResources();
		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
			VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		bool hasStencilComponent(VkFormat format);

		void createDepthResources();
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
			VkImageLayout newLayout, uint32_t mipLevels);

		void createFramebuffers();

		// We are managing multiple textures using ModelObject struct
		void createTextureImages();
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		
		void createTextureImageViews();
		void createTextureSamplers();
		void loadModels();

		void createVertexBuffers();
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void createIndexBuffers();
	};

	static std::vector<char> readFile(const std::string& filename);
}