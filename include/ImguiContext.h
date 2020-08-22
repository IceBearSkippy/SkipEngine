#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <array>
#include <imgui.h>
#include <string>
#include <fstream>
#include <vector>
#include <Camera.h>
namespace Skip {

    // Options and values to display/toggle from the UI
    struct UISettings {
        bool displayModels = true;
        bool displayLogos = true;
        bool displayBackground = true;
        bool animateLight = false;
        float lightSpeed = 0.25f;
        std::array<float, 50> frameTimes{};
        float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
        float lightTimer = 0.0f;
    };

    // TODO add to initializer class
    struct Buffer {
        VkDevice device;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDescriptorBufferInfo descriptor;
        VkDeviceSize size = 0;
        VkDeviceSize alignment = 0;
        void* mapped = nullptr;
        /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
        VkBufferUsageFlags usageFlags;
        /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
        VkMemoryPropertyFlags memoryPropertyFlags;
        void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();
        VkResult bind(VkDeviceSize offset = 0);
        void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void copyTo(void* data, VkDeviceSize size);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void destroy();
    };

    // IMGUI Class
    class ImguiContext {
    public:
        struct UISettings uiSettings;
        struct PushConstBlock {
            glm::vec2 scale;
            glm::vec2 translate;
        } pushConstBlock;

        ImguiContext();
        ~ImguiContext();
        void DestroyImguiContext(VkDevice device);
        void init(float width, float height);
        void initResources(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderPass, VkQueue copyQueue, VkCommandPool commandPool, const std::string& shadersPath, VkSampleCountFlagBits msaaSamples);
        void newFrame(std::string title, std::string gpuDeviceName, float frameTimer, bool updateFrameGraph, Camera* camera);
        void updateBuffers(VkDevice device, VkPhysicalDevice physicalDevice);
        void drawFrame(VkCommandBuffer commandBuffer);

        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;
        VkDescriptorSet descriptorSet;
        VkPipelineCache pipelineCache;
    private:
        // Vulkan resources for rendering the UI
        VkSampler sampler;
        Buffer vertexBuffer;
        Buffer indexBuffer;
        int32_t vertexCount = 0;
        int32_t indexCount = 0;
        VkDeviceMemory fontMemory = VK_NULL_HANDLE;
        VkImage fontImage = VK_NULL_HANDLE;
        VkImageView fontView = VK_NULL_HANDLE;
        
        
        
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        
    };

    // TODO: Move these helper methods to new file
    std::vector<char> readFile(const std::string& filename);
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
    void endSingleTimeCommands(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

    void transitionImageLayout(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue,
        VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    bool hasStencilComponent(VkFormat format);

    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageAspectFlags aspectMask,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask);

    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkImageSubresourceRange subresourceRange,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask);


    void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
        VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkResult createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
        Skip::Buffer* buffer, VkDeviceSize size, void* data = nullptr);
}