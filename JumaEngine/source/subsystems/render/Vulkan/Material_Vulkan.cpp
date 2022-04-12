﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptions_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "Shader_Vulkan.h"
#include "Texture_Vulkan.h"
#include "RenderTarget_Vulkan.h"
#include "subsystems/render/Shader.h"
#include "subsystems/render/VertexBuffer.h"
#include "subsystems/render/RenderTarget.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanFramebuffer.h"
#include "vulkanObjects/VulkanImage.h"
#include "vulkanObjects/VulkanRenderImage.h"
#include "vulkanObjects/VulkanRenderPass.h"

namespace JumaEngine
{
    Material_RenderAPIObject_Vulkan::~Material_RenderAPIObject_Vulkan()
    {
        clearVulkanData();
    }

    bool Material_RenderAPIObject_Vulkan::initInternal()
    {
        if (!createDescriptorPool())
        {
            clearVulkanData();
            return false;
        }
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::createDescriptorPool()
    {
        constexpr int8 maxRenderedFrameCount = RenderSubsystem_RenderAPIObject_Vulkan::getMaxRenderFrameCount();

        uint32 bufferUniformCount = 0;
        uint32 imageUniformCount = 0;
        for (const auto& uniformData : m_Parent->getShader()->getUniforms())
        {
            switch (uniformData.value.type)
            {
            case ShaderUniformType::Mat4:
                {
                    bufferUniformCount++;

                    jarray<UniformValue_Buffer>& values = m_UniformValues_Buffer[uniformData.key];
                    values.resize(maxRenderedFrameCount);
                    for (auto& value : values)
                    {
                        value.buffer = getRenderSubsystemObject()->createVulkanObject<VulkanBuffer>();
                    }
                }
                break;
            case ShaderUniformType::Texture:
            case ShaderUniformType::RenderTarget:
                {
                    imageUniformCount++;

                    jarray<UniformValue_Image>& values = m_UniformValues_Image[uniformData.key];
                    values.resize(maxRenderedFrameCount);
                }
                break;

            default:
                JUMA_LOG(error, JSTR("Unknown uniform type"));
                return false;
            }
        }

        jarray<VkDescriptorPoolSize> poolSizes;
        if (bufferUniformCount > 0)
        {
            VkDescriptorPoolSize& poolSize = poolSizes.addDefault();
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = bufferUniformCount * maxRenderedFrameCount;
        }
        if (imageUniformCount > 0)
        {
            VkDescriptorPoolSize& poolSize = poolSizes.addDefault();
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = imageUniformCount * maxRenderedFrameCount;
        }
        if (poolSizes.isEmpty())
        {
            // No uniforms at all
            return true;
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32>(poolSizes.getSize());
        poolInfo.pPoolSizes = poolSizes.getData();
        poolInfo.maxSets = static_cast<uint8>(maxRenderedFrameCount);
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        const VkResult result = vkCreateDescriptorPool(getRenderSubsystemObject()->getDevice(), &poolInfo, nullptr, &m_DescriptorPool);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create descriptor pool"), result);
            return false;
        }

        m_DescriptorSets.resize(maxRenderedFrameCount);
        return true;
    }

    bool Material_RenderAPIObject_Vulkan::bindDescriptorSet(VkCommandBuffer commandBuffer, const int8 frameIndex)
    {
        if (m_DescriptorPool == nullptr)
        {
            return true;
        }
        if (!updateDescriptorSet(frameIndex))
        {
            return false;
        }

        const Shader_RenderAPIObject_Vulkan* shader = dynamic_cast<const Shader_RenderAPIObject_Vulkan*>(m_Parent->getShader()->getRenderAPIObject());
        vkCmdBindDescriptorSets(
            commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getPipelineLayout(),
            0, 1, &m_DescriptorSets[frameIndex].descriptorSet, 0, nullptr
        );
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::updateDescriptorSet(const int8 frameIndex)
    {
        if (!createDescriptorSet(frameIndex))
        {
            return false;
        }

        DescriptorSetContainer& container = m_DescriptorSets[frameIndex];
        if (container.valid)
        {
            return true;
        }

        jarray<VkDescriptorBufferInfo> bufferInfos;
        jarray<VkWriteDescriptorSet> descriptorWrites;
        bufferInfos.reserve(m_UniformValues_Buffer.getSize());
        descriptorWrites.reserve(bufferInfos.getSize());

        for (const auto& uniform : m_Parent->getShader()->getUniforms())
        {
            VkWriteDescriptorSet descriptorWrite{};
            switch (uniform.value.type)
            {
            case ShaderUniformType::Mat4:
                {
                    VkDescriptorBufferInfo bufferInfo{};

                    bool valueUpdated = false;
                    switch (uniform.value.type)
                    {
                    case ShaderUniformType::Mat4:
                        valueUpdated = updateBufferUniformValue<ShaderUniformType::Mat4>(uniform.key, frameIndex, bufferInfo);
                        break;
                    default: ;
                    }
                    if (!valueUpdated)
                    {
                        continue;
                    }

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfos.add(bufferInfo);
                }
                break;
            case ShaderUniformType::Texture:
                {
                    VkDescriptorImageInfo imageInfo{};
                    if (!updateTextureUniformValue(uniform.key, frameIndex, imageInfo))
                    {
                        continue;
                    }

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;
                }
                break;
            case ShaderUniformType::RenderTarget:
                {
                    VkDescriptorImageInfo imageInfo{};
                    if (!updateRenderTargetUniformValue(uniform.key, frameIndex, imageInfo))
                    {
                        continue;
                    }

                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;
                }
                break;

            default: ;
            }

            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = container.descriptorSet;
            descriptorWrite.dstBinding = uniform.value.location;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrites.add(descriptorWrite);
        }
        if (!descriptorWrites.isEmpty())
        {
            vkUpdateDescriptorSets(getRenderSubsystemObject()->getDevice(), 
               static_cast<uint32>(descriptorWrites.getSize()), descriptorWrites.getData(),
               0, nullptr
            );
        }

        container.valid = true;
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::createDescriptorSet(const int8 frameIndex)
    {
        if (!m_DescriptorSets.isValidIndex(frameIndex))
        {
            JUMA_LOG(warning, JSTR("Invalid frame index"));
            return false;
        }
        if (m_DescriptorSets[frameIndex].descriptorSet != nullptr)
        {
            return true;
        }

        const Shader* shader = m_Parent->getShader();
        const Shader_RenderAPIObject_Vulkan* shader_object = dynamic_cast<const Shader_RenderAPIObject_Vulkan*>(shader->getRenderAPIObject());
        VkDescriptorSetLayout layout = shader_object->getDescriptorSetLayout();

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = m_DescriptorPool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;
        const VkResult result = vkAllocateDescriptorSets(getRenderSubsystemObject()->getDevice(), &allocateInfo, &m_DescriptorSets[frameIndex].descriptorSet);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to allocate descriptor set"), result);
            return false;
        }
        return true;
    }

    bool Material_RenderAPIObject_Vulkan::updateBufferUniformValueData(UniformValue_Buffer* bufferValue, const uint64 dataSize, const void* data, 
        VkDescriptorBufferInfo& outInfo)
    {
        if (!bufferValue->buffer->isValid())
        {
            const bool success = bufferValue->buffer->init(dataSize, 
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {}, 
                VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
            if (!success)
            {
                JUMA_LOG(error, JSTR("Failed to initialize uniform value buffer"));
                return false;
            }
        }
        if (!bufferValue->buffer->setData(data, dataSize))
        {
            JUMA_LOG(error, JSTR("Failed to update uniform value buffer"));
            return false;
        }

        outInfo.buffer = bufferValue->buffer->get();
        outInfo.offset = 0;
        outInfo.range = dataSize;
        return true;
    }

    bool Material_RenderAPIObject_Vulkan::updateTextureUniformValue(const jstringID& name, const int8 frameIndex, VkDescriptorImageInfo& outInfo)
    {
        if (m_Parent->isOverrideParam(name))
        {
            jarray<UniformValue_Image>* images = m_UniformValues_Image.find(name);
            UniformValue_Image* imageValue = images != nullptr ? images->findByIndex(frameIndex) : nullptr;

            if (!imageValue->valid)
            {
                Texture* texture = nullptr;
                if (m_Parent->getParamValue<ShaderUniformType::Texture>(name, texture))
                {
                    const Texture_RenderAPIObject_Vulkan* textureObject = texture != nullptr ? texture->getRenderAPIObject<Texture_RenderAPIObject_Vulkan>() : nullptr;
                    VulkanImage* image = textureObject != nullptr ? textureObject->getImage() : nullptr;
                    if (image == nullptr)
                    {
                        /* TODO: Handle this case, return false for now */
                        return false;
                    }

                    imageValue->image = image;
                    imageValue->valid = true;

                    outInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    outInfo.imageView = image->getImageView();
                    outInfo.sampler = image->getSampler();
                    return true;
                }
            }
        }
        else if (m_Parent->isMaterialInstance())
        {
            Material_RenderAPIObject_Vulkan* baseMaterialObject = dynamic_cast<Material_RenderAPIObject_Vulkan*>(m_Parent->getBaseMaterial()->getRenderAPIObject());
            return baseMaterialObject->updateTextureUniformValue(name, frameIndex, outInfo);
        }
        return false;
    }
    bool Material_RenderAPIObject_Vulkan::updateRenderTargetUniformValue(const jstringID& name, const int8 frameIndex, VkDescriptorImageInfo& outInfo)
    {
        if (m_Parent->isOverrideParam(name))
        {
            jarray<UniformValue_Image>* images = m_UniformValues_Image.find(name);
            UniformValue_Image* imageValue = images != nullptr ? images->findByIndex(frameIndex) : nullptr;
            if (!imageValue->valid)
            {
                RenderTarget* renderTarget = nullptr;
                if (m_Parent->getParamValue<ShaderUniformType::RenderTarget>(name, renderTarget))
                {
                    const RenderTarget_RenderAPIObject_Vulkan* renderTargetObject = renderTarget != nullptr ? renderTarget->getRenderAPIObject<RenderTarget_RenderAPIObject_Vulkan>() : nullptr;
                    const VulkanFramebuffer* framebuffer = renderTargetObject != nullptr ? renderTargetObject->getFramebuffer(frameIndex) : nullptr;
                    VulkanImage* image = framebuffer != nullptr ? framebuffer->getResultImage() : nullptr;
                    if (image == nullptr)
                    {
                        /* TODO: Handle this case, return false for now */
                        return false;
                    }

                    imageValue->image = image;
                    imageValue->valid = true;

                    outInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    outInfo.imageView = image->getImageView();
                    outInfo.sampler = image->getSampler();
                    return true;
                }
            }
        }
        else if (m_Parent->isMaterialInstance())
        {
            Material_RenderAPIObject_Vulkan* baseMaterialObject = dynamic_cast<Material_RenderAPIObject_Vulkan*>(m_Parent->getBaseMaterial()->getRenderAPIObject());
            return baseMaterialObject->updateRenderTargetUniformValue(name, frameIndex, outInfo);
        }
        return false;
    }

    void Material_RenderAPIObject_Vulkan::onMaterialParamChanged(const jstringID& paramName)
    {
        const ShaderUniformType type = m_Parent->getParamType(paramName);
        switch (type)
        {
        case ShaderUniformType::Mat4:
            {
                for (auto& valueBuffer : m_UniformValues_Buffer[paramName])
                {
                    valueBuffer.valid = false;
                }
            }
            break;
        case ShaderUniformType::Texture:
        case ShaderUniformType::RenderTarget:
            {
                for (auto& valueBuffer : m_UniformValues_Image[paramName])
                {
                    valueBuffer.valid = false;
                }
            }
            break;

        default: ;
        }
        for (auto& descriptorSet : m_DescriptorSets)
        {
            descriptorSet.valid = false;
        }
    }

    void Material_RenderAPIObject_Vulkan::clearVulkanData()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();

        for (auto& vertexPipelines : m_RenderPipelines)
        {
            for (auto& pipeline : vertexPipelines.value)
            {
                vkDestroyPipeline(device, pipeline.value, nullptr);
            }
        }
        m_RenderPipelines.clear();

        for (auto& value : m_UniformValues_Buffer)
        {
            for (auto& frameValue : value.value)
            {
                if (frameValue.buffer != nullptr)
                {
                    delete frameValue.buffer;
                    frameValue.buffer = nullptr;
                }
            }
        }
        m_UniformValues_Buffer.clear();

        if (m_DescriptorPool != nullptr)
        {
            m_DescriptorSets.clear();
            vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
            m_DescriptorPool = nullptr;
        }
    }

    bool Material_RenderAPIObject_Vulkan::bindRenderPipeline(VkCommandBuffer commandBuffer, const jstringID& vertexName, 
        const VulkanRenderPass* renderPass)
    {
        VkPipeline pipeline;
        if (!createRenderPipeline(vertexName, renderPass, pipeline))
        {
            return false;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::createRenderPipeline(const jstringID& vertexName, const VulkanRenderPass* renderPass, VkPipeline& outPipeline)
    {
        const render_pass_id_type renderPassID = renderPass != nullptr ? renderPass->getTypeID() : INVALID_RENDER_PASS_TYPE_ID;
        if (renderPassID == INVALID_RENDER_PASS_TYPE_ID)
        {
            JUMA_LOG(error, JSTR("Invaid render pass ID"));
            return false;
        }

        auto& vertexPipelines = m_RenderPipelines[vertexName];
        {
            VkPipeline* pipeline = vertexPipelines.find(renderPassID);
            if (pipeline != nullptr)
            {
                outPipeline = *pipeline;
                return true;
            }
        }

        const VertexDescription_Vulkan* vertexDescription = getRenderSubsystemObject()->findVertexDescription(vertexName);
        if (vertexDescription == nullptr)
        {
            JUMA_LOG(error, JSTR("Invaid vertex description"));
            return false;
        }

        // Shader stages
        const Shader_RenderAPIObject_Vulkan* shader = dynamic_cast<const Shader_RenderAPIObject_Vulkan*>(m_Parent->getShader()->getRenderAPIObject());
        const jarray<VkPipelineShaderStageCreateInfo>& shaderStageInfos = shader->getPipelineStageInfos();

        // Vertex input data
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &vertexDescription->binding;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexDescription->attributes.getSize());
        vertexInputInfo.pVertexAttributeDescriptions = vertexDescription->attributes.getData();

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

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

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
        multisampling.rasterizationSamples = renderPass->getDescription().sampleCount;
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

        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;

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
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = shader->getPipelineLayout();
        pipelineInfo.renderPass = renderPass->get();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline renderPipeline;
        const VkResult result = vkCreateGraphicsPipelines(getRenderSubsystemObject()->getDevice(), nullptr, 1, &pipelineInfo, nullptr, &renderPipeline);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create render pipeline"), result);
            return false;
        }

        vertexPipelines[renderPassID] = renderPipeline;
        outPipeline = renderPipeline;
        return true;
    }

    bool Material_RenderAPIObject_Vulkan::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        if ((vertexBuffer == nullptr) || (renderOptions == nullptr))
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        const RenderOptions_Vulkan* options = reinterpret_cast<const RenderOptions_Vulkan*>(renderOptions);
        const VulkanCommandBuffer* commandBuffer = options->commandBuffer;
        const int8 frameIndex = getRenderSubsystemObject()->getRenderFrameIndex();
        if (!bindRenderPipeline(commandBuffer->get(), vertexBuffer->getVertexName(), options->renderPass) ||
            !bindDescriptorSet(commandBuffer->get(), frameIndex))
        {
            return false;
        }
        return vertexBuffer->render(renderOptions);
    }
}

#endif
