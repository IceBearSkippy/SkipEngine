#include <VulkanManager.h>

namespace Skip {

    
    std::vector<const char*> DEFAULT_VALIDATION = {
        "VK_LAYER_KHRONOS_validation"
    };

    VulkanManager::VulkanManager(VulkanWindow* window, SkipScene* scene, bool enableValidationLayers)
        : _validationLayers(DEFAULT_VALIDATION){
        _window = window;
        _enableValidationLayers = enableValidationLayers;
        _instance = VK_NULL_HANDLE;

        this->createInstance();
        this->setupDebugMessenger();
        this->createSurface();
        this->queryPhysicalDevices();

        _vulkanDevice = new VulkanDevice(this->pickPhysicalDevice());

        this->createLogicalDevice();

        _vulkanSwapchain = new VulkanSwapchain(_vulkanDevice, _window, scene);

        this->setupImGUI();
    }

    VulkanManager::~VulkanManager() {
        
        if (_enableValidationLayers) {
            destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        }
        _vulkanSwapchain->~VulkanSwapchain();
        if (_window->_surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(_instance, _window->_surface, nullptr);;
        }

        _window->~VulkanWindow();
        if (_instance != VK_NULL_HANDLE) {
            vkDestroyInstance(_instance, nullptr);
        }
        
    }

    void VulkanManager::drawFrame(uint32_t currentImage) {
        _vulkanSwapchain->drawFrame(currentImage);
    }

    bool VulkanManager::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char* layerName : _validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    VkResult VulkanManager::createDebugUtilsMessengerEXT(VkInstance instance, const
        VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
        VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
        pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator,
                pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanManager::destroyDebugUtilsMessengerEXT(VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger, const
        VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;

    }

    void VulkanManager::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    std::vector<const char*> VulkanManager::getRequiredExtensions() {
        //GLFW extensions!
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    void VulkanManager::createInstance() {
        if (_enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not available");
        }
        // instance is connection between app and Vulkan library
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = ENGINE_VERSION; // is this the version?
        appInfo.pEngineName = ENGINE_NAME.c_str();
        appInfo.engineVersion = ENGINE_VERSION;
        appInfo.apiVersion = ENGINE_API_VERSION;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        auto glfwExtensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
        createInfo.ppEnabledExtensionNames = glfwExtensions.data();


        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void VulkanManager::setupDebugMessenger() {
        if (!_enableValidationLayers) return;
        VkDebugUtilsMessengerCreateInfoEXT debugMessCreateInfo;
        populateDebugMessengerCreateInfo(debugMessCreateInfo);

        if (createDebugUtilsMessengerEXT(_instance, &debugMessCreateInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to setup debug messenger!");
        }
    }

    void VulkanManager::createSurface() {
        if (glfwCreateWindowSurface(_instance, _window->_glfw, nullptr, &(_window->_surface)) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void VulkanManager::queryPhysicalDevices() {
        // Grab physical devices available and store into vectore _gpuDevices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        //Family indices might need to be added to gpu info
        for (const auto& device : devices) {
            GPUInfo gpu = createGPUInfo(device);
            _gpuDevices.push_back(gpu);
        }

        // sort based on score
        std::sort(_gpuDevices.begin(), _gpuDevices.end(), [this](GPUInfo const& a, GPUInfo const& b) {
            return a.score > b.score;
        });
    }

    GPUInfo VulkanManager::createGPUInfo(VkPhysicalDevice device) {
        GPUInfo gpuInfo;

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

        VkSampleCountFlagBits msaaSamples = this->getMaxUsableSampleCount(device);

        int score = 0;
        // Discrete GPUs have a significant performance advantage
        // Add any other properties of interest
        if (deviceProperties.deviceType ==
            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;
        gpuInfo.device = device;
        gpuInfo.features = deviceFeatures;
        gpuInfo.properties = deviceProperties;
        gpuInfo.msaaSamples = getMaxUsableSampleCount(device);
        gpuInfo.score = score;
        return gpuInfo;
    }

    VkSampleCountFlagBits VulkanManager::getMaxUsableSampleCount(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
            physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;

    }

    GPUInfo* VulkanManager::pickPhysicalDevice() {
        // we pick the first device we have by default
        if (_gpuDevices.size() > 0) {
            return &_gpuDevices.front();
        } else {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    

    void VulkanManager::createLogicalDevice() {
        QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(_vulkanDevice->_gpuInfo, _window->_surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        std::set<uint32_t> uniqueQueueFamilies =
        { indices.graphicsFamily.value(), indices.presentFamily.value() };
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }


        //specify device features like geometry shaders and anisotropy
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        //deviceFeatures.sampleRateShading = VK_TRUE; // enable simple shading feature

        // create logical device
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        // enable extensions - currently enable swap chain
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (_enableValidationLayers) {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        // instantiate the logical device
        if (vkCreateDevice(_vulkanDevice->getPhysicalDevice(), &createInfo, nullptr, &_vulkanDevice->_logicalDevice) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }
        vkGetDeviceQueue(_vulkanDevice->_logicalDevice, indices.presentFamily.value(), 0, &_vulkanDevice->_queues.graphics);
        vkGetDeviceQueue(_vulkanDevice->_logicalDevice, indices.presentFamily.value(), 0, &_vulkanDevice->_queues.present);
    }

    void VulkanManager::setupImGUI() {
        // TODO: Upload fonts to GPU, main loop and describe UI

        QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(_vulkanDevice->_gpuInfo, _window->_surface);
        ImGui_ImplGlfw_InitForVulkan(_window->_glfw, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = _instance;
        init_info.PhysicalDevice = _vulkanDevice->getPhysicalDevice();
        init_info.Device = *_vulkanDevice->getLogicalDevice();
        init_info.QueueFamily = queueFamilyIndices.graphicsFamily.value();
        init_info.Queue = _vulkanDevice->_queues.graphics;
        init_info.PipelineCache = nullptr; // we don't have pipeline cache right now
        init_info.DescriptorPool = _vulkanSwapchain->_descriptorPool;
        init_info.Allocator = nullptr;
        init_info.MinImageCount = _vulkanSwapchain->_swapChainImages.size();
        init_info.ImageCount = _vulkanSwapchain->_swapChainImages.size();
        init_info.CheckVkResultFn = nullptr;

        // Need to make separate render pass for GUI to be drawn over your rendering
        VkAttachmentDescription attachment = {};
        attachment.format = _vulkanSwapchain->_swapChainImageFormat;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &attachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;
        if (vkCreateRenderPass(*_vulkanDevice->getLogicalDevice(), &info, nullptr, &_vulkanSwapchain->_imGuiRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("Could not create Dear ImGui's render pass");
        }

        ImGui_ImplVulkan_Init(&init_info, _vulkanSwapchain->_renderPass);
    }
}