#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include <array>
#include <fstream>
#include <chrono>
#include <unordered_map>

#include <VulkanDevice.h>
#include <VulkanWindow.h>
#include <objects/SkipObject.h>
namespace Skip {

    struct SwapchainDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanSwapchain {

    public:
        VulkanSwapchain();
        VulkanSwapchain(VulkanDevice* vkDevice, VulkanWindow* vkWindow, std::vector<SkipObject*> skipObjects);
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
        std::vector<SkipObject*>  _skipObjects;
        SwapchainDetails querySwapchain();

        VkDescriptorPool _descriptorPool;
        std::vector<VkDescriptorSet> _descriptorSets;
        std::vector<VkCommandBuffer> _commandBuffers;

        //semaphores
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;
        size_t _currentFrame = 0;

        //defines how many frames to be processed concurrently
        //note: each frame should have its own set of semaphores
        const int MAX_FRAMES_IN_FLIGHT = 2;

        //handle resizing
        bool _framebufferResized = false;

        uint32_t stageFrame();
        void updateUniformBuffers(uint32_t currentImage);
        void drawFrame(uint32_t currentImage);
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
        void loadObjects();

        void createVertexBuffers();
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void createIndexBuffers();

        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();
    };

    static std::vector<char> readFile(const std::string& filename);
}