#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <VulkanSwapchain.h>
#include <vulkan/vulkan.h>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

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
    } uiSettings;


    class ImguiContext {
    public:
        struct PushConstBlock {
            glm::vec2 scale;
            glm::vec2 translate;
        } pushConstBlock;
        ImguiContext();
        ImguiContext(VulkanSwapchain* vulkanSwapchain);
        ~ImguiContext();
        void init(float width, float height);
        void initResources(VkRenderPass renderPass, VkQueue copyQueue, const std::string& shadersPath);
        void newFrame(bool updateFrameGraph, float currentTime);
        void updateBuffers();
        void drawFrame(VkCommandBuffer commandBuffer);

    private:
        // Vulkan resources for rendering the UI
        VkSampler sampler;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        void* vertMapped = nullptr;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        void* indexMapped = nullptr;
        int32_t vertexCount = 0;
        int32_t indexCount = 0;
        VkDeviceMemory fontMemory = VK_NULL_HANDLE;
        VkImage fontImage = VK_NULL_HANDLE;
        VkImageView fontView = VK_NULL_HANDLE;
        VkPipelineCache pipelineCache;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;
        VulkanDevice* _vulkanDevice;
        VulkanSwapchain* _vulkanSwapchain;
    };
}