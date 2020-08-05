#include <ImguiContext.h>

namespace Skip {
    ImguiContext::ImguiContext() {

    }

    ImguiContext::ImguiContext(VulkanSwapchain* vulkanSwapchain) {
        _vulkanSwapchain = vulkanSwapchain;
        _vulkanDevice = _vulkanSwapchain->_vkDevice;

    }

    ImguiContext::~ImguiContext() {
        VkDevice device = *_vulkanDevice->getLogicalDevice();
        ImGui::DestroyContext();
        // Release all Vulkan resources required for rendering imGui
        vertexBuffer.destroy();
        indexBuffer.destroy();
        vkDestroyImage(device, fontImage, nullptr);
        vkDestroyImageView(device, fontView, nullptr);
        vkFreeMemory(device, fontMemory, nullptr);
        vkDestroySampler(device, sampler, nullptr);
        vkDestroyPipelineCache(device, pipelineCache, nullptr);
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }

    void ImguiContext::init(float width, float height) {
        // Color scheme
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
        style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        // Dimensions
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(width, height);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    void ImguiContext::initResources(VkRenderPass renderPass, VkQueue copyQueue, const std::string& shadersPath) {
        ImGuiIO& io = ImGui::GetIO();
        VkDevice device = *_vulkanDevice->getLogicalDevice();

        // Create font texture
        unsigned char* fontData;
        int texWidth, texHeight;
        io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
        VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

        // Create target image for copy
        VkImageCreateInfo imageInfo{};
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent.width = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (vkCreateImage(device, &imageInfo, nullptr, &fontImage) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create font image!");
        }
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, fontImage, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = _vulkanSwapchain->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkAllocateMemory(device, &allocInfo, nullptr, &fontMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate image memory");
        }
        vkBindImageMemory(device, fontImage, fontMemory, 0);

        // Image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.image = fontImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device, &viewInfo, nullptr, &fontView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture image view!");
        }

        // Staging buffers for font data upload

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        _vulkanSwapchain->createBuffer(uploadSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, uploadSize, 0, &data);
        memcpy(data, fontData, (size_t)uploadSize);
        vkUnmapMemory(device, stagingBufferMemory);


        // Copy buffer data to font image
        VkCommandBuffer copyCmd = _vulkanSwapchain->beginSingleTimeCommands();

        // Prepare for transfer
        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkImageLayout oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout newImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_HOST_BIT;
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = aspectMask;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = fontImage;
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        // Put barrier inside setup command buffer
        vkCmdPipelineBarrier(
            copyCmd,
            srcStageMask,
            dstStageMask,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);


        // Copy
        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageExtent.width = texWidth;
        bufferCopyRegion.imageExtent.height = texHeight;
        bufferCopyRegion.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(
            copyCmd,
            stagingBuffer,
            fontImage,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &bufferCopyRegion
        );

        // Prepare for shader read
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        oldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        newImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        srcStageMask = VK_PIPELINE_STAGE_HOST_BIT;
        dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = aspectMask;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = fontImage;
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(
            copyCmd,
            srcStageMask,
            dstStageMask,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

        _vulkanSwapchain->endSingleTimeCommands(copyCmd);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
        //stagingBuffer.destroy();

        // Font texture Sampler
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create a texture sampler!");
        }
        // Descriptor pool
        std::vector<VkDescriptorPoolSize> poolSizes = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
        };
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 2;
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool!");
        }
        
        // Descriptor set layout
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        setLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        setLayoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings[0].binding = 0;
        setLayoutBindings[0].descriptorCount = 1;

        VkDescriptorSetLayoutCreateInfo descriptorLayout{};
        descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayout.pBindings = setLayoutBindings.data();
        descriptorLayout.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());

        if (vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout!");
        }

        // Descriptor set
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocated descriptor sets!");
        }
        
        VkDescriptorImageInfo fontDescriptor{};
        fontDescriptor.sampler = sampler;
        fontDescriptor.imageView = fontView;
        fontDescriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        
        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pImageInfo = &fontDescriptor;
        writeDescriptorSet.descriptorCount = 1;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets = { writeDescriptorSet };

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

        // Pipeline cache
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        if (vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline cache!");
        }
        // Pipeline layout
        // Push constants for UI rendering parameters
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = sizeof(PushConstBlock);
        pushConstantRange.size = 0;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        // Setup graphics pipeline for UI rendering
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyState.flags = 0;
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterizationState{};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_NONE;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationState.flags = 0;
        rasterizationState.depthClampEnable = VK_FALSE;
        rasterizationState.lineWidth = 1.0f;

        // Enable blending
        VkPipelineColorBlendAttachmentState blendAttachmentState{};
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &blendAttachmentState;

        VkPipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.depthTestEnable = VK_FALSE;
        depthStencilState.depthWriteEnable = VK_FALSE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        viewportState.flags = 0;

        VkPipelineMultisampleStateCreateInfo multisampleState{};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.flags = 0;

        std::vector<VkDynamicState> dynamicStateEnables = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStateEnables.data();
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
        dynamicState.flags = 0;

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.flags = 0;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineCreateInfo.pStages = shaderStages.data();

        // Vertex bindings an attributes based on ImGui vertex definition
        VkVertexInputBindingDescription vInputBindDescription{};
        vInputBindDescription.binding = 0;
        vInputBindDescription.stride = sizeof(ImDrawVert);
        vInputBindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
            vInputBindDescription,
        };

        // Location 0: Position
        VkVertexInputAttributeDescription vInputAttribDescriptionPosition{};
        vInputAttribDescriptionPosition.location = 0;
        vInputAttribDescriptionPosition.binding = 0;
        vInputAttribDescriptionPosition.format = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescriptionPosition.offset = offsetof(ImDrawVert, pos);

        // Location 1: UV
        VkVertexInputAttributeDescription vInputAttribDescriptionUV{};
        vInputAttribDescriptionUV.location = 0;
        vInputAttribDescriptionUV.binding = 1;
        vInputAttribDescriptionUV.format = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescriptionUV.offset = offsetof(ImDrawVert, uv);

        // Location 2: Color
        VkVertexInputAttributeDescription vInputAttribDescriptionColor{};
        vInputAttribDescriptionColor.location = 0;
        vInputAttribDescriptionColor.binding = 2;
        vInputAttribDescriptionColor.format = VK_FORMAT_R8G8B8A8_UNORM;
        vInputAttribDescriptionColor.offset = offsetof(ImDrawVert, col);

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
            vInputAttribDescriptionPosition,
            vInputAttribDescriptionUV,
            vInputAttribDescriptionColor,
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        //TODO: create ui vert and frag shaders
        auto vertShaderCode = readFile("resources/imgui/ui.vert.spv");
        auto fragShaderCode = readFile("resources/imgui/ui.frag.spv");

        VkShaderModule vertShaderModule = _vulkanSwapchain->createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = _vulkanSwapchain->createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;

        vertShaderStageInfo.pName = "main";

        vertShaderStageInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        fragShaderStageInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
        shaderStages[0] = vertShaderStageInfo;
        shaderStages[1] = fragShaderStageInfo;
        if (vkCreateGraphicsPipelines(device, pipelineCache, 1,
            &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create imgui graphics pipeline!");
        }
    }

    // Starts a new imGui frame and sets up windows and ui elements
    void ImguiContext::newFrame(bool updateFrameGraph, float currentTime) {
        ImGui::NewFrame();

        // Init imGui windows and elements

        ImVec4 clear_color = ImColor(114, 144, 154);
        static float f = 0.0f;
        ImGui::TextUnformatted(_vulkanSwapchain->_vkWindow->_title.c_str());
        ImGui::TextUnformatted(_vulkanDevice->_gpuInfo->properties.deviceName);

        // Update frame time display
        if (updateFrameGraph) {
            std::rotate(uiSettings.frameTimes.begin(), uiSettings.frameTimes.begin() + 1, uiSettings.frameTimes.end());
            float frameTime = 1000.0f / (currentTime * 1000.0f);
            uiSettings.frameTimes.back() = frameTime;
            if (frameTime < uiSettings.frameTimeMin) {
                uiSettings.frameTimeMin = frameTime;
            }
            if (frameTime > uiSettings.frameTimeMax) {
                uiSettings.frameTimeMax = frameTime;
            }
        }

        ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

        ImGui::Text("Camera");
        
        ImGui::InputFloat3("position x", &_vulkanSwapchain->_scene->_camera->_position.x, 2);
        ImGui::InputFloat3("rotation y", &_vulkanSwapchain->_scene->_camera->_position.y, 2);
        
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Example settings");
        ImGui::Checkbox("Render models", &uiSettings.displayModels);
        ImGui::Checkbox("Display logos", &uiSettings.displayLogos);
        ImGui::Checkbox("Display background", &uiSettings.displayBackground);
        ImGui::Checkbox("Animate light", &uiSettings.animateLight);
        ImGui::SliderFloat("Light speed", &uiSettings.lightSpeed, 0.1f, 1.0f);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();

        // Render to generate draw buffers
        ImGui::Render();
    }

    // Update vertex and index buffer containing the imGui elements when required
    void ImguiContext::updateBuffers() {
        VkDevice device = *_vulkanDevice->getLogicalDevice();
        ImDrawData* imDrawData = ImGui::GetDrawData();

        // Note: Alignment is done inside buffer creation
        VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
        VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

        if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
            return;
        }

        // Update buffers only if vertex or index count has been changed compared to current buffer size

        // Vertex buffer
        if ((vertexBuffer == VK_NULL_HANDLE) || (vertexCount != imDrawData->TotalVtxCount)) {
            //vertexBuffer.unmap()
            if (vertMapped) {
                vkUnmapMemory(device, vertexBufferMemory);
                vertMapped = nullptr;
            }

            //vertexBuffer.destroy();
            if (vertexBuffer) {
                vkDestroyBuffer(device, vertexBuffer, nullptr);
            }
            if (vertexBufferMemory) {
                vkFreeMemory(device, vertexBufferMemory, nullptr);
            }
            // Create the buffer handle
            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.size = vertexBufferSize;

            if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create vertex buffer!");
            }

            // Create the memory backing up the buffer handle
            VkMemoryRequirements memReqs;
            VkMemoryAllocateInfo memAlloc{};
            memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

            vkGetBufferMemoryRequirements(device, vertexBuffer, &memReqs);
            memAlloc.allocationSize = memReqs.size;
            memAlloc.memoryTypeIndex = _vulkanSwapchain->findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            if (vkAllocateMemory(device, &memAlloc, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate image memory");
            }

            // If a pointer to the buffer data has been passed, map the buffer and copy over the data
            void* data;
            if (data != nullptr) {
                vkMapMemory(device, vertexBufferMemory, 0, vertexBufferSize, 0, &vertMapped);
                memcpy(vertMapped, data, vertexBufferSize);
                if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
                    VkMappedMemoryRange mappedRange = {};
                    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                    mappedRange.memory = vertexBufferMemory;
                    mappedRange.offset = 0;
                    mappedRange.size = vertexBufferSize;
                    vkFlushMappedMemoryRanges(device, 1, &mappedRange);
                }

                if (vertMapped) {
                    vkUnmapMemory(device, vertexBufferMemory);
                    vertMapped = nullptr;
                }
            }
            vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
            vertexCount = imDrawData->TotalVtxCount;
            //vertexBuffer.map();
            vkMapMemory(device, vertexBufferMemory, 0, vertexBufferSize, 0, &vertMapped);
        }

        // Index buffer
        if ((indexBuffer == VK_NULL_HANDLE) || (indexCount < imDrawData->TotalIdxCount)) {
            //indexBuffer.unmap()
            if (indexMapped) {
                vkUnmapMemory(device, indexBufferMemory);
                indexMapped = nullptr;
            }

            //indexBuffer.destroy();
            if (indexBuffer) {
                vkDestroyBuffer(device, indexBuffer, nullptr);
            }
            if (indexBufferMemory) {
                vkFreeMemory(device, indexBufferMemory, nullptr);
            }
            // Create the buffer handle
            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            bufferCreateInfo.size = indexBufferSize;

            if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &indexBuffer) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create vertex buffer!");
            }

            // Create the memory backing up the buffer handle
            VkMemoryRequirements memReqs;
            VkMemoryAllocateInfo memAlloc{};
            memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

            vkGetBufferMemoryRequirements(device, indexBuffer, &memReqs);
            memAlloc.allocationSize = memReqs.size;
            memAlloc.memoryTypeIndex = _vulkanSwapchain->findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            if (vkAllocateMemory(device, &memAlloc, nullptr, &indexBufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate image memory");
            }

            // If a pointer to the buffer data has been passed, map the buffer and copy over the data
            void* data;
            if (data != nullptr) {
                vkMapMemory(device, indexBufferMemory, 0, indexBufferSize, 0, &indexMapped);
                memcpy(indexMapped, data, vertexBufferSize);
                if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
                    VkMappedMemoryRange mappedRange = {};
                    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                    mappedRange.memory = indexBufferMemory;
                    mappedRange.offset = 0;
                    mappedRange.size = indexBufferSize;
                    vkFlushMappedMemoryRanges(device, 1, &mappedRange);
                }

                if (indexMapped) {
                    vkUnmapMemory(device, indexBufferMemory);
                    indexMapped = nullptr;
                }
            }
            vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);
            indexCount = imDrawData->TotalVtxCount;
            //indexBuffer.map();
            vkMapMemory(device, vertexBufferMemory, 0, vertexBufferSize, 0, &indexMapped);
        }


        // Upload data
        ImDrawVert* vtxDst = (ImDrawVert*)vertMapped;
        ImDrawIdx* idxDst = (ImDrawIdx*)indexMapped;

        for (int n = 0; n < imDrawData->CmdListsCount; n++) {
            const ImDrawList* cmd_list = imDrawData->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Flush to make writes visible to GPU
        //vertexBuffer.flush();
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = vertexBufferMemory;
        mappedRange.offset = 0;
        mappedRange.size = vertexBufferSize;
        vkFlushMappedMemoryRanges(device, 1, &mappedRange);

        //indexBuffer.flush();
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = indexBufferMemory;
        mappedRange.offset = 0;
        mappedRange.size = indexBufferSize;
        vkFlushMappedMemoryRanges(device, 1, &mappedRange);
    }

    void ImguiContext::drawFrame(VkCommandBuffer commandBuffer) {

    }
}