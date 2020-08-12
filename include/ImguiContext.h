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
    private:
        // Vulkan resources for rendering the UI
        VkSampler sampler;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
        void* vertMapped = nullptr;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
        void* indexMapped = nullptr;
        int32_t vertexCount = 0;
        int32_t indexCount = 0;
        VkDeviceMemory fontMemory = VK_NULL_HANDLE;
        VkImage fontImage = VK_NULL_HANDLE;
        VkImageView fontView = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache;
        
        
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        
    };

    // TODO: Move these helper methods to new file
    std::vector<char> readFile(const std::string& filename);
    uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
        VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
    void endSingleTimeCommands(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
}