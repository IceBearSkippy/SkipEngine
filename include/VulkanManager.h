#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include <cstring>

#include <VulkanSwapchain.h>

const std::string ENGINE_NAME = "SkipEngine";
const uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 2, 1);

const uint32_t ENGINE_API_VERSION = VK_API_VERSION_1_2;

namespace Skip {

    class VulkanWindow;

    class SkipObject;

    struct QueueFamilyIndices;
    struct GPUInfo;
    struct Queues;

    struct SwapchainDetails;

    struct UniformBufferObject;

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class VulkanManager {
    public:
        VulkanManager(VulkanWindow* window, std::vector<SkipObject*> skipObjects, bool enableValidationLayers);
        ~VulkanManager();

        bool checkValidationLayerSupport();

        void drawFrame(uint32_t currentImage);

        VulkanWindow* _window;
        bool _enableValidationLayers;
        std::vector<const char*> _validationLayers;
        std::vector<const char*> getRequiredExtensions();

        VulkanDevice* _vulkanDevice;
        VulkanSwapchain* _vulkanSwapchain;
    private:
        void createInstance();
        void createSurface();
        void createLogicalDevice();

        VkDebugUtilsMessengerEXT _debugMessenger;
        void setupDebugMessenger();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkInstance _instance = VK_NULL_HANDLE;
        VkResult createDebugUtilsMessengerEXT(VkInstance instance, const
            VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
            VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
            pDebugMessenger);
        void destroyDebugUtilsMessengerEXT(VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger, const
            VkAllocationCallbacks* pAllocator);

        std::vector<GPUInfo> _gpuDevices;
        void queryPhysicalDevices();
        VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice device);
        GPUInfo createGPUInfo(VkPhysicalDevice device);
        GPUInfo* pickPhysicalDevice();

    };


}
