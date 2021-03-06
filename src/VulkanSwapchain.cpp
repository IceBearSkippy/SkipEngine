#include <VulkanSwapchain.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Skip {

    VulkanSwapchain::VulkanSwapchain() {};

    VulkanSwapchain::VulkanSwapchain(VulkanDevice* vkDevice, VulkanWindow* vkWindow, VkInstance* instance, SkipScene* scene) {
        _vkDevice = vkDevice;
        _vkWindow = vkWindow;
        _instance = instance;
        _scene = scene;
        _currentFrame = 0;

        this->createSwapChain();
        this->createImageViews();
        this->createRenderPass();
        this->createPipelineCache();
        this->createDescriptorSetLayout();
        this->createCommandPool();

        this->createGraphicsPipeline();

        this->initImgui();

        this->createColorResources();
        this->createDepthResources();
        this->createFramebuffers();
        this->createTextureImages();
        this->createTextureImageViews();
        this->createTextureSamplers();
        this->loadObjects();
        this->createVertexBuffers();
        this->createIndexBuffers();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSets();
        this->createSyncObjects();

        this->allocateCommandBuffers();
        this->buildCommandBuffers();
        
    };

    VulkanSwapchain::~VulkanSwapchain() {

        vkDeviceWaitIdle(*_vkDevice->getLogicalDevice());
        this->cleanupSwapChain();
        
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        _imguiContext->DestroyImguiContext(logicalDevice);
        
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            vkDestroySampler(logicalDevice, _scene->_objects[i]->_textureSampler, nullptr);
            vkDestroyImageView(logicalDevice, _scene->_objects[i]->_textureImageView, nullptr);

            vkDestroyImage(logicalDevice, _scene->_objects[i]->_textureImage, nullptr);
            vkFreeMemory(logicalDevice, _scene->_objects[i]->_textureImageMemory, nullptr);

            if (_scene->_objects[i]->_useIndexBuffer) {
                vkDestroyBuffer(logicalDevice, _scene->_objects[i]->_indexBuffer, nullptr);
                vkFreeMemory(logicalDevice, _scene->_objects[i]->_indexBufferMemory, nullptr);
            }
            vkDestroyBuffer(logicalDevice, _scene->_objects[i]->_vertexBuffer, nullptr);
            vkFreeMemory(logicalDevice, _scene->_objects[i]->_vertexBufferMemory, nullptr);
        }

        vkDestroyDescriptorSetLayout(logicalDevice, _descriptorSetLayout, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(logicalDevice, _renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, _imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice, _inFlightFences[i], nullptr);
        }
        vkDestroyPipelineCache(logicalDevice, _pipelineCache, nullptr);
        vkDestroyCommandPool(logicalDevice, _commandPool, nullptr);
        vkDestroyDevice(logicalDevice, nullptr);
    };

    uint32_t VulkanSwapchain::stageFrame() {
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        //wait for the frame to be finished
        vkWaitForFences(logicalDevice, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

        //Aquire image from swap chain
        uint32_t imageIndex;
        // imageAvailableSemaphore is to be signaled when presentation engine is
        // finished using engine (VK_NULL_HANDLE could be fence)
        vkAcquireNextImageKHR(logicalDevice, _swapChain, UINT64_MAX,
            _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

        // Check if a previous frame is using this image (ie theres a fence to wait on)
        if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(logicalDevice, 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        _imagesInFlight[imageIndex] = _inFlightFences[_currentFrame];

        return imageIndex;
    }

    void VulkanSwapchain::drawFrame(uint32_t currentImage, float deltaTime) {
        //TODO debug this draw frame for each frame

        this->buildCommandBuffers();

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();

        _frameTimer = (float)deltaTime / 1000.0f;
        //Submitting the command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        //specify which command buffers to actually submit for execution
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[currentImage];

        // specify which semaphore (renderFinishedSemaphore) to signal
        // once the command buffer has finished executing
        VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // reset fence before using it
        vkResetFences(logicalDevice, 1, &_inFlightFences[_currentFrame]);
        if (vkQueueSubmit(_vkDevice->_queues.graphics, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        // Presentation
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        // specify which semaphores to wait on before presentation can happen
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        // specify swap chains to present images to and the index
        // of the image for each swap chain. This will always be 1
        VkSwapchainKHR swapChains[] = { _swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &currentImage;

        // Specify an array of VkResult values to check for every
        // individual swap chain.
        presentInfo.pResults = nullptr; //optional

        VkResult result = vkQueuePresentKHR(_vkDevice->_queues.present, &presentInfo);
        vkQueueWaitIdle(_vkDevice->_queues.present);
        // gives condition if presentation queue is optimal/suboptimal
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            _framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    }

    void VulkanSwapchain::updateUniformBuffers(uint32_t currentImage) {

        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            void* data;
            vkMapMemory(*_vkDevice->getLogicalDevice(), _scene->_objects[i]->_mvpUboBuffersMemory[currentImage], 0,
                sizeof(_scene->_objects[i]->_mvpUBO), 0, &data);
            memcpy(data, &_scene->_objects[i]->_mvpUBO, sizeof(_scene->_objects[i]->_mvpUBO));
            vkUnmapMemory(*_vkDevice->getLogicalDevice(), _scene->_objects[i]->_mvpUboBuffersMemory[currentImage]);

            vkMapMemory(*_vkDevice->getLogicalDevice(), _scene->_objects[i]->_lightUboBuffersMemory[currentImage], 0,
                sizeof(_scene->_objects[i]->_lightUBO), 0, &data);
            memcpy(data, &_scene->_objects[i]->_lightUBO, sizeof(_scene->_objects[i]->_lightUBO));
            vkUnmapMemory(*_vkDevice->getLogicalDevice(), _scene->_objects[i]->_lightUboBuffersMemory[currentImage]);
        }
    }

    void VulkanSwapchain::createPipelineCache() {
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        if (vkCreatePipelineCache(*_vkDevice->getLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &_pipelineCache) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline cache!");
        }
    }

    // recreateSwapChain is called when we draw frames
    void VulkanSwapchain::recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(_vkWindow->_glfw, &width, &height);
        while (width == 0 || height == 0) {
            // minimize makes framebuffer size = 0
            // we wait until window is maximized to proceed
            glfwGetFramebufferSize(_vkWindow->_glfw, &width, &height);
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(*_vkDevice->getLogicalDevice());

        this->cleanupSwapChain();
        this->createSwapChain();
        this->createImageViews();
        this->createRenderPass();
        this->createGraphicsPipeline();
        this->createColorResources();
        this->createDepthResources();
        this->createFramebuffers();
        this->createUniformBuffers();
        this->createDescriptorPool();
        this->createDescriptorSets();
        this->allocateCommandBuffers();

    }

    void VulkanSwapchain::cleanupSwapChain() {
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();

        vkDestroyImageView(logicalDevice, _colorImageView, nullptr);
        vkDestroyImage(logicalDevice, _colorImage, nullptr);
        vkFreeMemory(logicalDevice, _colorImageMemory, nullptr);

        vkDestroyImageView(logicalDevice, _depthImageView, nullptr);
        vkDestroyImage(logicalDevice, _depthImage, nullptr);
        vkFreeMemory(logicalDevice, _depthImageMemory, nullptr);

        for (size_t i = 0; i < _swapChainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(logicalDevice, _swapChainFramebuffers[i], nullptr);
        }
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            for (size_t j = 0; j < _swapChainImages.size(); j++) {
                vkDestroyBuffer(logicalDevice, _scene->_objects[i]->_mvpUboBuffers[j], nullptr);
                vkFreeMemory(logicalDevice, _scene->_objects[i]->_mvpUboBuffersMemory[j], nullptr);

                vkDestroyBuffer(logicalDevice, _scene->_objects[i]->_lightUboBuffers[j], nullptr);
                vkFreeMemory(logicalDevice, _scene->_objects[i]->_lightUboBuffersMemory[j], nullptr);
            }
        }
        vkDestroyDescriptorPool(logicalDevice, _descriptorPool, nullptr);
        vkFreeCommandBuffers(logicalDevice, _commandPool, static_cast<uint32_t>(_commandBuffers.size()),
            _commandBuffers.data());
        vkDestroyPipeline(logicalDevice, _graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, _pipelineLayout, nullptr);

        vkDestroyRenderPass(logicalDevice, _renderPass, nullptr);
        for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
            vkDestroyImageView(logicalDevice, _swapChainImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(logicalDevice, _swapChain, nullptr);
    }

    SwapchainDetails VulkanSwapchain::querySwapchain() {
        VkPhysicalDevice device = _vkDevice->getPhysicalDevice();
        VkSurfaceKHR& surface = _vkWindow->_surface;
        SwapchainDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        //settle for the first format found
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VulkanWindow* vkWindow) {
        GLFWwindow* window = vkWindow->_glfw;
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
            actualExtent.width =
                std::max(capabilities.minImageExtent.width,
                    std::min(capabilities.maxImageExtent.width,
                        actualExtent.width));
            actualExtent.height =
                std::max(capabilities.minImageExtent.height,
                    std::min(capabilities.maxImageExtent.height,
                        actualExtent.height));
            return actualExtent;
        }
    }

    void VulkanSwapchain::createSwapChain() {
        // Builds the following member variables:
        //     _swapChain
        //     _swapChainImages
        //     _swapChainImageFormat
        //     _swapChainExtent

        SwapchainDetails swapchainDetails = querySwapchain();
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainDetails.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainDetails.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapchainDetails.capabilities, _vkWindow);

        uint32_t imageCount = swapchainDetails.capabilities.minImageCount + 1;

        if (swapchainDetails.capabilities.maxImageCount > 0 && imageCount >
            swapchainDetails.capabilities.maxImageCount) {
            imageCount = swapchainDetails.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _vkWindow->_surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        // number of layers each image consists of (usually 1 unless steroscopic 3d app)
        createInfo.imageArrayLayers = 1;
        // Specifies kind of operations we'll use the images in the swap chain for
        // Could change image usage to perform operations like post-processing (VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = QueueFamilyIndices::findQueueFamilies(_vkDevice->_gpuInfo, _vkWindow->_surface);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            // Images can be used across multiple queue families without explicit ownership transfers
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            // image is owned by one queue family at a time and ownership must be explicitly tranferred before
            // using it in another queue family. Offers best performance
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapchainDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        // if clipped set to true, we don't care about color of pixels
        // that are obscured === better performance
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        // Would want to keep track of previous swap chain just in case
        // swap chain becomes invalid. Leave for now
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain!");
        }

        // get swap chain images
        vkGetSwapchainImagesKHR(logicalDevice, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(logicalDevice, _swapChain, &imageCount, _swapChainImages.data());

        _swapChainImageFormat = surfaceFormat.format;
        _swapChainExtent = extent;
    }

    void VulkanSwapchain::createImageViews() {
        //resize list to fit all image views we'll be creating
        // Builds the following member variables:
        //     _swapChainImageViews
        _swapChainImageViews.resize(_swapChainImages.size());
        for (size_t i = 0; i < _swapChainImages.size(); i++) {
            _swapChainImageViews[i] = createImageView(_swapChainImages[i], _swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }
    }

    VkImageView VulkanSwapchain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
        uint32_t mipLevels) {
        // helper function
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        // viewInfo.components initialization is 0 (default) because VK_COMPONENT_SWIZZLE_IDENTITY

        VkImageView imageView;
        if (vkCreateImageView(*_vkDevice->getLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture image view!");
        }
        return imageView;
    }

    VkFormat VulkanSwapchain::findSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features) {
        // helper function
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("Failed to find supported format!");
    }

    VkFormat VulkanSwapchain::findDepthFormat() {
        // helper function
        return findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }


    void VulkanSwapchain::createRenderPass() {
        // Builds the following member variables:
        //     _renderPass
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = _swapChainImageFormat;
        colorAttachment.samples = _vkDevice->_gpuInfo->msaaSamples;
        // final layout must resolve to regular image
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // we dont care what the previous layout image was in

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = _vkDevice->_gpuInfo->msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = _swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // this will resolve layout

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        //specify reference to subpass
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        // the index of attachment in this array is directly referenced from the
        // fragment shader with layout(location = 0)out vec4 outColor
        // Can make direct references to shader
        //subpass.pInputAttachments;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;
        //subpass.pPreserveAttachments;

        // Subpass dependencies
        // refer to before or after the render pass depending on whether
        // it is specified in srcSubpass or dstSubpass
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0; // refers to our subpass (which is the first and only one)
        //specify operations to wait on / stages in which these operations occur
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        // these settings will prevent the transitioning to happen unless it's actually neccessary
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
        // Render pass
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(*_vkDevice->getLogicalDevice(), &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void VulkanSwapchain::createDescriptorSetLayout() {
        // Builds the following member variables:
        //     _descriptorSetLayout

        // Every binding needs to be described
        // mvp binding
        VkDescriptorSetLayoutBinding mvpLayoutBinding{};
        mvpLayoutBinding.binding = 0;
        mvpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpLayoutBinding.descriptorCount = 1;
        // specify shader stage. If all -- STAGE_ALL_GRAPHICS
        // but here we're only referencing vertex shader
        mvpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        // Relevant for image sampling related descriptors
        mvpLayoutBinding.pImmutableSamplers = nullptr;

        //Sampler binding
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding lightLayoutBinding{};
        lightLayoutBinding.binding = 2;
        lightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightLayoutBinding.descriptorCount = 1;
        lightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        lightLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings = { mvpLayoutBinding, samplerLayoutBinding, lightLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(*_vkDevice->getLogicalDevice(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }
    }

    void VulkanSwapchain::createGraphicsPipeline() {
        // Builds the following member variables:
        //     _pipelineLayout
        //     _graphicsPipeline
        VkDevice device = *_vkDevice->getLogicalDevice();
        auto vertShaderCode = readFile("resources/shaders/vert.spv");
        auto fragShaderCode = readFile("resources/shaders/frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

        vertShaderStageInfo.module = vertShaderModule;
        // use different entrypoints to combine multiple fragment shaders into one shader module
        vertShaderStageInfo.pName = "main";

        // pSpecializationInfo -- specify values for shader constants
        vertShaderStageInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        fragShaderStageInfo.pSpecializationInfo = nullptr;

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages { 
            vertShaderStageInfo,
            fragShaderStageInfo
        };
        // create vertex input
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        // vertexBindingDescriptions point to an array of structs that describe loading vertex data
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        // vertexAttributeDescriptions point to an array of structs that describe loading vertex data
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // create input assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //Viewports and scissors
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_swapChainExtent.width;
        viewport.height = (float)_swapChainExtent.height;
        viewport.minDepth = 0.0f; // depth between [0..1]
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = _swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        //Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // optional
        rasterizer.depthBiasClamp = 0.0f; // optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // optional

        // multisampling -- one of the ways to perform anti-aliasing
        // enabling it requires enabling GPU feature
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE; // can enable if enabled from logical device
        multisampling.minSampleShading = 1.0f; // (0.2f) min fraction for simple shading; closer to one is smoother
        multisampling.rasterizationSamples = _vkDevice->_gpuInfo->msaaSamples;
        multisampling.pSampleMask = nullptr; // optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // optional
        multisampling.alphaToOneEnable = VK_FALSE; // optional

        //VkPipelineDepthStencilStateCreateInfo -- only if you're using a depth/stencil buffer

        // Color blending
        // common way to use color blending is to implement alpha blending
        // We are currently not blending.
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE; // VK_TRUE - alpha blending
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional -- VK_BLEND_FACTOR_SRC_ALPHA - alpha blending
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional -- VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA - alpha blending
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // optional
        colorBlending.blendConstants[1] = 0.0f; // optional
        colorBlending.blendConstants[2] = 0.0f; // optional
        colorBlending.blendConstants[3] = 0.0f; // optional


        // dynamic states -- not included currently
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

        // depth stencil enable
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; // comparison to keep or discard fragments. Lower depth = closer
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // optional
        depthStencil.maxDepthBounds = 1.0f; // optional

        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // optional
        depthStencil.back = {}; // optional


        // create pipelineLayout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // setting descriptor layout for binding info
        pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        // create Graphics Pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size()); // what is this for?
        pipelineInfo.pStages = shaderStages.data();

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil; // optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState; // optional

        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional -- can switch between pipelines, but right now there's only one
        pipelineInfo.basePipelineIndex = -1; // optional

        if (vkCreateGraphicsPipelines(logicalDevice, _pipelineCache, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
        vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
    }

    void VulkanSwapchain::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(_vkDevice->_gpuInfo, _vkWindow->_surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        if (vkCreateCommandPool(*_vkDevice->getLogicalDevice(), &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    void VulkanSwapchain::createColorResources() {
        VkFormat colorFormat = _swapChainImageFormat;

        createImage(_swapChainExtent.width, _swapChainExtent.height, 1, _vkDevice->_gpuInfo->msaaSamples, colorFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _colorImage, _colorImageMemory);
        _colorImageView = createImageView(_colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void VulkanSwapchain::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
        //helper function
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D; //1D for gradients, 3D for voxel volumes
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = numSamples;
        imageInfo.flags = 0;

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();

        if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(_vkDevice->getPhysicalDevice(), memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory");
        }

        vkBindImageMemory(logicalDevice, image, imageMemory, 0);
    }

    void VulkanSwapchain::createDepthResources() {
        // Same resolution as color attachment/swap chain extent
        VkFormat depthFormat = findDepthFormat();
        createImage(_swapChainExtent.width, _swapChainExtent.height, 1, _vkDevice->_gpuInfo->msaaSamples, depthFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImage, _depthImageMemory);
        _depthImageView = createImageView(_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        VkDevice device = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();

        transitionImageLayout(device, physicalDevice, _commandPool, _vkDevice->_queues.graphics, _depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }


    void VulkanSwapchain::createFramebuffers() {
        // need to create framebuffers for all vkimages in swap chains
        _swapChainFramebuffers.resize(_swapChainImageViews.size());

        //iterate and create framebuffers from imageviews
        for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
            std::array<VkImageView, 3> attachments = {
                _colorImageView,
                _depthImageView,
                _swapChainImageViews[i]

            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = _swapChainExtent.width;
            framebufferInfo.height = _swapChainExtent.height;
            framebufferInfo.layers = 1; // our swap chain images are single images

            if (vkCreateFramebuffer(*_vkDevice->getLogicalDevice(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }

    void VulkanSwapchain::createTextureImages() {
        // There are going to be many texture images to process
        // In this engine, we'll start with using the ModelObject struct
        // to load an object and uv texture map
        //
        // Model Objects MUST contain a texture
        // need a default texturePath

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            int texWidth, texHeight, texChannels;


            stbi_uc* pixels = stbi_load(_scene->_objects[i]->_texturePath.c_str(), &texWidth,
                &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;

            _scene->_objects[i]->_mipLevels = static_cast<uint32_t>(floor(log2(std::max(texWidth, texHeight)))) + 1;

            if (!pixels) {
                throw std::runtime_error("Failed to load texture image!");
            }

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(physicalDevice, logicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                stagingBufferMemory);

            void* data;
            vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(logicalDevice, stagingBufferMemory);

            stbi_image_free(pixels);

            createImage(texWidth, texHeight, _scene->_objects[i]->_mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _scene->_objects[i]->_textureImage, _scene->_objects[i]->_textureImageMemory);

            // image layout to transfer (pipeline barrier)
            transitionImageLayout(logicalDevice, physicalDevice, _commandPool, _vkDevice->_queues.graphics, _scene->_objects[i]->_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _scene->_objects[i]->_mipLevels);

            //copy to staging buffer
            copyBufferToImage(stagingBuffer, _scene->_objects[i]->_textureImage,
                static_cast<uint32_t>(texWidth),
                static_cast<uint32_t>(texHeight));
            // prepare image for shader access
            // moved the VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL transitioning to mipmap method
            // TODO: can we make mipmapping optional?
            generateMipmaps(_scene->_objects[i]->_textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, _scene->_objects[i]->_mipLevels);

            vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
        }

    }

    void VulkanSwapchain::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkDevice device = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, _commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        endSingleTimeCommands(device, _vkDevice->_queues.graphics, _commandPool, commandBuffer);
    }

    void VulkanSwapchain::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
        int32_t texHeight, uint32_t mipLevels) {
        VkDevice device = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        // first check if image format supports linear blitting
        // there are alternatives to handle different formats
        // It's uncommon to generate mipmap levels at runtime... They are usually
        // pregenerated and stored in the texture file
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("Texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, _commandPool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;


        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            // add vk image blit{}
            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;
            vkCmdBlitImage(commandBuffer,
                image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            // end of loop, we divide mip dimensions by 2
            if (mipWidth > 1) {
                mipWidth /= 2;
            }
            if (mipHeight > 1) {
                mipHeight /= 2;
            }
        }

        // this barrier handles the last transition
        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        endSingleTimeCommands(device, _vkDevice->_queues.graphics, _commandPool, commandBuffer);
    }

    void VulkanSwapchain::createTextureImageViews() {
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            _scene->_objects[i]->_textureImageView = createImageView(_scene->_objects[i]->_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_ASPECT_COLOR_BIT, _scene->_objects[i]->_mipLevels);
        }
    }

    void VulkanSwapchain::createTextureSamplers() {
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            // specify how to interpolate texels -- other option is VK_FILTER_NEAREST
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            // U, V, W is x, y, z in texture space
            // can specify repeat or clamp here
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            //anisotropic filtering
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = 16.0f;
            // border color when sampling beyond image (can't specify arbitrary color)
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            // if true: [0, texWidth) and [0, texHeight)
            // else: [0, 1) on all axis
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            // mainly used for percentage-closer filtering on shadow maps
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            // mipmapping settings
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            // using the higher mip map levels will result in more blurry (as in for distance)
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(_scene->_objects[i]->_mipLevels); // Max level of detail

            if (vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &_scene->_objects[i]->_textureSampler) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create a texture sampler!");
            }
        }
    }


    void VulkanSwapchain::loadObjects() {
        float aspect = _swapChainExtent.width / (float)_swapChainExtent.height;
        _scene->loadScene(aspect);
    }

    void VulkanSwapchain::createVertexBuffers() {
        // This currently creates buffer and memory buffer in ModelObject

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            VkDeviceSize bufferSize = sizeof(_scene->_objects[i]->_vertices[0]) * _scene->_objects[i]->_vertices.size();
            // TRANSFER_SRC - source of transfer
            createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer, stagingBufferMemory);
            // filling the vertex buffer by first passing a staging buffer
            // could specify special vaule VK_WHOLE_SIZE to map all memory (3rd param)
            // Caching can be an issue which can be fixed with vkFlushedMappedMemoryRanges
            // after writing to mapped memory or use a memory heap that is host coherent
            void* data;
            vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, _scene->_objects[i]->_vertices.data(), (size_t)bufferSize);
            vkUnmapMemory(logicalDevice, stagingBufferMemory);

            // TRANSFER_DST - transfer destination
            createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _scene->_objects[i]->_vertexBuffer, _scene->_objects[i]->_vertexBufferMemory);

            copyBuffer(stagingBuffer, _scene->_objects[i]->_vertexBuffer, bufferSize);

            vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

        }
    }

    void VulkanSwapchain::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        // Memory transfer operations use command buffers --> we need a temp command buffer
        // TODO: You might want to create a separate command pool for these short lived copy/transfers.
        //       In that case, use VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag in command pool
        VkDevice device = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, _commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(device, _vkDevice->_queues.graphics, _commandPool, commandBuffer);
    }

    void VulkanSwapchain::createIndexBuffers() {
        // Index buffers are only needed if normals/lightings are not needed as a per vertex

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            // TODO Add check for if vbo or using indices
            if (_scene->_objects[i]->_useIndexBuffer) {
                VkDeviceSize bufferSize = sizeof(_scene->_objects[i]->_indices[0]) * _scene->_objects[i]->_indices.size();
                // TRANSFER_SRC - source of transfer
                createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    stagingBuffer, stagingBufferMemory);

                // filling the vertex buffer by first passing a staging buffer
                // could specify special vaule VK_WHOLE_SIZE to map all memory (3rd param)
                // Caching can be an issue which can be fixed with vkFlushedMappedMemoryRanges
                // after writing to mapped memory or use a memory heap that is host coherent
                void* data;
                vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
                memcpy(data, _scene->_objects[i]->_indices.data(), (size_t)bufferSize);
                vkUnmapMemory(logicalDevice, stagingBufferMemory);

                // TRANSFER_DST - transfer destination
                createBuffer(physicalDevice, logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _scene->_objects[i]->_indexBuffer, _scene->_objects[i]->_indexBufferMemory);

                copyBuffer(stagingBuffer, _scene->_objects[i]->_indexBuffer, bufferSize);

                vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
                vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
            } 
        }
    }

    void VulkanSwapchain::createUniformBuffers() {
        // Currently using one buffer for each skip object
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        VkPhysicalDevice physicalDevice = _vkDevice->getPhysicalDevice();
        VkDeviceSize mvpbufferSize = sizeof(MvpBufferObject);
        VkDeviceSize lightBufferSize = sizeof(LightBufferObject);
        for (size_t i = 0; i < _scene->_objects.size(); i++) {
            _scene->_objects[i]->_mvpUboBuffers.resize(_swapChainImages.size());
            _scene->_objects[i]->_mvpUboBuffersMemory.resize(_swapChainImages.size());

            _scene->_objects[i]->_lightUboBuffers.resize(_swapChainImages.size());
            _scene->_objects[i]->_lightUboBuffersMemory.resize(_swapChainImages.size());

            for (size_t j = 0; j < _swapChainImages.size(); j++) {
                createBuffer(physicalDevice, logicalDevice, mvpbufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _scene->_objects[i]->_mvpUboBuffers[j],
                    _scene->_objects[i]->_mvpUboBuffersMemory[j]);

                createBuffer(physicalDevice, logicalDevice, lightBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _scene->_objects[i]->_lightUboBuffers[j],
                    _scene->_objects[i]->_lightUboBuffersMemory[j]);
            }
        }


    }

    void VulkanSwapchain::createDescriptorPool() {
        // describe descriptor types our sets are going to contain
        // Create pools for each ubos and sampler
        std::array<VkDescriptorPoolSize, 4> poolSizes{};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());

        poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());

        // TODO: Is this needed? This pool is used for imgui
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[3].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());


        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(_swapChainImages.size()) + 1; // +1 for imgui
        // structure has optional flag to determine individual descriptor sets
        // can be freed or not: VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
        poolInfo.flags = 0;

        if (vkCreateDescriptorPool(*_vkDevice->getLogicalDevice(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
    }

    void VulkanSwapchain::createDescriptorSets() {
        // We'll create descriptor set for each SkipObject

        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        std::vector<VkDescriptorSetLayout> layouts(_swapChainImages.size(), _descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(_scene->_objects.size());
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(_swapChainImages.size());
        if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocated descriptor sets!");
        }

        // descriptor sets need to be configured for each buffer
        for (size_t i = 0; i < _scene->_objects.size(); i++) {

            VkDescriptorBufferInfo mvpBufferInfo{};
            mvpBufferInfo.buffer = *_scene->_objects[i]->_mvpUboBuffers.data();
            mvpBufferInfo.offset = 0;
            mvpBufferInfo.range = sizeof(_scene->_objects[i]->_mvpUboBuffers);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _scene->_objects[i]->_textureImageView;
            imageInfo.sampler = _scene->_objects[i]->_textureSampler;

            VkDescriptorBufferInfo lightBufferInfo{};
            lightBufferInfo.buffer = *_scene->_objects[i]->_lightUboBuffers.data();
            lightBufferInfo.offset = 0;
            lightBufferInfo.range = sizeof(_scene->_objects[i]->_lightUboBuffers);

            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = _descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0; // not using array
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &mvpBufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional -- refer to image data
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional -- refer to buffer views

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = _descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0; // not using array
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;
            descriptorWrites[1].pTexelBufferView = nullptr;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = _descriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0; // not using array
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pBufferInfo = &lightBufferInfo;
            descriptorWrites[2].pImageInfo = nullptr;
            descriptorWrites[2].pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanSwapchain::allocateCommandBuffers() {
        _commandBuffers.resize(_swapChainFramebuffers.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();
        if (vkAllocateCommandBuffers(*_vkDevice->getLogicalDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void VulkanSwapchain::buildCommandBuffers() {

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapChainExtent;
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        _imguiContext->newFrame("test", "GPU_NAME", _frameTimer, true, _scene->_camera);
        
        _imguiContext->updateBuffers(*_vkDevice->getLogicalDevice(), _vkDevice->getPhysicalDevice());

        // Command Buffer Recording
        for (size_t i = 0; i < _commandBuffers.size(); i++) {

            renderPassInfo.framebuffer = _swapChainFramebuffers[i];

            if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer!");
            }
            
            vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.width = _swapChainExtent.width;
            viewport.height = _swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(_commandBuffers[i], 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.extent.width = _swapChainExtent.width;
            scissor.extent.height = _swapChainExtent.height;
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            vkCmdSetScissor(_commandBuffers[i], 0, 1, &scissor);

            //Basic Drawing Commands
            vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[i], 0, nullptr);
            vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
            
            for (size_t j = 0; j < _scene->_objects.size(); j++) {
                VkDeviceSize offsets[] = { 0 };
                
                vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, &_scene->_objects[j]->_vertexBuffer, offsets);

                if (_scene->_objects[j]->_useIndexBuffer) {
                    vkCmdBindIndexBuffer(_commandBuffers[i], _scene->_objects[j]->_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                    vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1,
                        &_descriptorSets[j], 0, nullptr);

                    vkCmdDrawIndexed(_commandBuffers[i], _scene->_objects[j]->_indices.size(), 1, 0, 0, 0);
                } else {
                    vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1,
                        &_descriptorSets[j], 0, nullptr);
                    vkCmdDraw(_commandBuffers[i], _scene->_objects[j]->_vertices.size(), 1, 0, 0);
                }

            }

            _imguiContext->drawFrame(_commandBuffers[i]);

            vkCmdEndRenderPass(_commandBuffers[i]);
            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer!");
            }
        }
    }

    void VulkanSwapchain::createSyncObjects() {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        _imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkDevice logicalDevice = *_vkDevice->getLogicalDevice();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr,
                &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr,
                    &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(logicalDevice, &fenceInfo, nullptr,
                    &_inFlightFences[i]) != VK_SUCCESS) {

                throw std::runtime_error("Failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanSwapchain::initImgui() {
        _imguiContext = new ImguiContext();
        _imguiContext->init((float)_swapChainExtent.width, (float)_swapChainExtent.height);
        _imguiContext->initResources(*_vkDevice->getLogicalDevice(), _vkDevice->getPhysicalDevice(), _renderPass, _vkDevice->_queues.graphics, _commandPool, "resources/shaders/imgui", _vkDevice->_gpuInfo->msaaSamples);
    }
    
}
