// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderPrimitive_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "Material_Vulkan.h"
#include "Shader_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "VertexBuffer_Vulkan.h"
#include "utils/jlog.h"
#include "vulkanObjects/VulkanSwapchain.h"
#include "RenderOptionsData_Vulkan.h"
#include "vulkanObjects/VulkanCommandBuffer.h"

namespace JumaEngine
{
    RenderPrimitive_Vulkan::~RenderPrimitive_Vulkan()
    {
        if (isValid())
        {
            clearVulkanData();
        }
    }

    bool RenderPrimitive_Vulkan::initInternal(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material)
    {
        if (!createRenderPipeline(vertexBuffer, material))
        {
            return false;
        }

        getRenderSubsystem()->getSwapchain()->onRenderPassChanged.bind(this, &RenderPrimitive_Vulkan::onRenderPassChanged);
        return true;
    }
    bool RenderPrimitive_Vulkan::createRenderPipeline(const jshared_ptr<VertexBuffer>& vertexBuffer, const jshared_ptr<Material>& material)
    {
        const VertexBuffer_Vulkan* vertexBufferVulkan = dynamic_cast<const VertexBuffer_Vulkan*>(vertexBuffer.get());
        const Shader_Vulkan* shaderVulkan = material != nullptr ? dynamic_cast<Shader_Vulkan*>(material->getShader().get()) : nullptr;
        if ((vertexBufferVulkan == nullptr) || (shaderVulkan == nullptr))
        {
            return false;
        }

        const jshared_ptr<VulkanSwapchain>& swapchain = getRenderSubsystem()->getSwapchain();
        const glm::uvec2 swapchainSize = swapchain->getSize();

        // Shader stages
        jarray<VkPipelineShaderStageCreateInfo> shaderStageInfos = shaderVulkan->createPipelineStageInfos();

        // Vertex input data
        const jarray<VkVertexInputBindingDescription>& bindingDescriptions = vertexBufferVulkan->getBindingDescriptions();
        const jarray<VkVertexInputAttributeDescription>& attributeDescriptions = vertexBufferVulkan->getAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32>(bindingDescriptions.getSize());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.getData();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.getSize());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.getData();

        // Geometry type
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        // Part of rendered image
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainSize.x);
        viewport.height = static_cast<float>(swapchainSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        // Part of screen
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { swapchainSize.x, swapchainSize.y };
        // Viewport combination
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = swapchain->getSampleCount();
        multisampling.minSampleShading = 0.2f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        // Blend for one framebuffer
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        // Global blend
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        //*VkDynamicState dynamicStates[] = {
	       // VK_DYNAMIC_STATE_VIEWPORT,
	       // VK_DYNAMIC_STATE_LINE_WIDTH
        //};
        //VkPipelineDynamicStateCreateInfo dynamicState = VkPipelineDynamicStateCreateInfo();
        //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        //dynamicState.dynamicStateCount = 2;
        //dynamicState.pDynamicStates = dynamicStates;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32>(shaderStageInfos.getSize());
        pipelineInfo.pStages = shaderStageInfos.getData();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = shaderVulkan->getPipelineLayout();
        pipelineInfo.renderPass = swapchain->getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex = -1;
        const VkResult result = vkCreateGraphicsPipelines(getRenderSubsystem()->getDevice(), nullptr, 1, &pipelineInfo, nullptr, &m_RenderPipeline);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(warning, JSTR("Failed to create render pipeline. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }
    bool RenderPrimitive_Vulkan::recreateRenderPipeline()
    {
        clearRenderPipeline();
        if (createRenderPipeline(getVertexBuffer(), getMaterial()))
        {
            m_RenderPipelineInvaid = false;
            return true;
        }
        return false;
    }
    void RenderPrimitive_Vulkan::onRenderPassChanged(VulkanSwapchain* swapchain)
    {
        m_RenderPipelineInvaid = true;
    }

    void RenderPrimitive_Vulkan::clearInternal()
    {
        clearVulkanData();
    }
    void RenderPrimitive_Vulkan::clearVulkanData()
    {
        getRenderSubsystem()->getSwapchain()->onRenderPassChanged.unbind(this, &RenderPrimitive_Vulkan::onRenderPassChanged);
        clearRenderPipeline();
        m_RenderPipelineInvaid = false;
    }
    void RenderPrimitive_Vulkan::clearRenderPipeline()
    {
        if (m_RenderPipeline != nullptr)
        {
            vkDestroyPipeline(getRenderSubsystem()->getDevice(), m_RenderPipeline, nullptr);
            m_RenderPipeline = nullptr;
        }
    }

    void RenderPrimitive_Vulkan::render(const RenderOptions& options)
    {
        if (!isValid())
        {
            return;
        }

        if (m_RenderPipelineInvaid)
        {
            recreateRenderPipeline();
        }

        const RenderOptionsData_Vulkan* data = options.getData<RenderOptionsData_Vulkan>();
        vkCmdBindPipeline(data->commandBuffer->get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_RenderPipeline);
        
        Material_Vulkan* material = dynamic_cast<Material_Vulkan*>(getMaterial().get());
        if (material->render(*data))
        {
            VertexBuffer_Vulkan* vertexBuffer = dynamic_cast<VertexBuffer_Vulkan*>(getVertexBuffer().get());
            vertexBuffer->render(*data);
        }
    }
}

#endif
