// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptions_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "Shader_Vulkan.h"
#include "Texture_Vulkan.h"
#include "RenderTarget_Vulkan.h"
#include "jutils/math/math_matrix.h"
#include "subsystems/render/Shader.h"
#include "subsystems/render/VertexBuffer.h"
#include "subsystems/render/RenderTarget.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanFramebuffer.h"
#include "vulkanObjects/VulkanImage.h"
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
        uint32 bufferUniformCount = 0;
        uint32 imageUniformCount = 0;
        for (const auto& uniformData : m_Parent->getShader()->getUniforms())
        {
            switch (uniformData.value.type)
            {
            case ShaderUniformType::Mat4:
                {
                    bufferUniformCount++;
                }
                break;
            case ShaderUniformType::Texture:
            case ShaderUniformType::RenderTarget:
                {
                    imageUniformCount++;
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
            poolSize.descriptorCount = bufferUniformCount;
        }
        if (imageUniformCount > 0)
        {
            VkDescriptorPoolSize& poolSize = poolSizes.addDefault();
            poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSize.descriptorCount = imageUniformCount;
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
        poolInfo.maxSets = 1;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        const VkResult result = vkCreateDescriptorPool(getRenderSubsystemObject()->getDevice(), &poolInfo, nullptr, &m_DescriptorPool);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create descriptor pool"), result);
            return false;
        }
        return true;
    }

    bool Material_RenderAPIObject_Vulkan::bindDescriptorSet(VkCommandBuffer commandBuffer)
    {
        if (m_DescriptorPool == nullptr)
        {
            return true;
        }
        if (!updateDescriptorSet())
        {
            return false;
        }

        const Shader_RenderAPIObject_Vulkan* shader = dynamic_cast<const Shader_RenderAPIObject_Vulkan*>(m_Parent->getShader()->getRenderAPIObject());
        vkCmdBindDescriptorSets(
            commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getPipelineLayout(),
            0, 1, &m_DescriptorSet, 0, nullptr
        );
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::updateDescriptorSet()
    {
        if (!createDescriptorSet())
        {
            return false;
        }

        jarray<VkDescriptorBufferInfo> bufferInfos;
        jarray<VkDescriptorImageInfo> imageInfos;
        jarray<VkWriteDescriptorSet> descriptorWrites;
        bufferInfos.reserve(m_UniformValues_Buffer_Mat4.getSize());
        imageInfos.reserve(m_UniformValues_Image.getSize());
        descriptorWrites.reserve(bufferInfos.getSize());
        for (const auto& uniform : m_Parent->getShader()->getUniforms())
        {
            VkWriteDescriptorSet descriptorWrite{};
            switch (uniform.value.type)
            {
            case ShaderUniformType::Mat4:
                {
                    math::matrix4 value;
                    if (!m_Parent->getParamValue<ShaderUniformType::Mat4>(uniform.key, value))
                    {
                        continue;
                    }

                    UniformValue_Buffer_Mat4* uniformValue = m_UniformValues_Buffer_Mat4.find(uniform.key);
                    if (uniformValue == nullptr)
                    {
                        uniformValue = &m_UniformValues_Buffer_Mat4[uniform.key];
                        uniformValue->buffer = getRenderSubsystemObject()->createVulkanObject<VulkanBuffer>();
                        const bool initSuccess = uniformValue->buffer->init(sizeof(math::matrix4), 
                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {},
                            VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                        );
                        if (!initSuccess)
                        {
                            JUMA_LOG(error, JSTR("Failed to initialize uniform value buffer"));
                            delete uniformValue->buffer;
                            m_UniformValues_Buffer_Mat4.remove(uniform.key);
                            continue;
                        }
                    }
                    else if (math::isMatricesEqual(value, uniformValue->value))
                    {
                        // No need to update
                        continue;
                    }

                    if (!uniformValue->buffer->setData(&value))
                    {
                        JUMA_LOG(error, JSTR("Failed to update uniform value buffer"));
                        continue;
                    }
                    uniformValue->value = value;

                    VkDescriptorBufferInfo& bufferInfo = bufferInfos.addDefault();
                    bufferInfo.buffer = uniformValue->buffer->get();
                    bufferInfo.offset = 0;
                    bufferInfo.range = sizeof(math::matrix4);
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfo;
                }
                break;

            case ShaderUniformType::Texture:
            case ShaderUniformType::RenderTarget:
                {
                    VulkanImage* imageValue;
                    if (uniform.value.type == ShaderUniformType::Texture)
                    {
                        Texture* texture = nullptr;
                        if (!m_Parent->getParamValue<ShaderUniformType::Texture>(uniform.key, texture))
                        {
                            continue;
                        }
                        const Texture_RenderAPIObject_Vulkan* textureObject = texture != nullptr ? texture->getRenderAPIObject<Texture_RenderAPIObject_Vulkan>() : nullptr;
                        imageValue = textureObject != nullptr ? textureObject->getImage() : nullptr; 
                    }
                    else
                    {
                        RenderTarget* renderTarget = nullptr;
                        if (!m_Parent->getParamValue<ShaderUniformType::RenderTarget>(uniform.key, renderTarget))
                        {
                            continue;
                        }
                        const RenderTarget_RenderAPIObject_Vulkan* renderTargetObject = renderTarget != nullptr ? renderTarget->getRenderAPIObject<RenderTarget_RenderAPIObject_Vulkan>() : nullptr;
                        const VulkanFramebuffer* framebuffer = renderTargetObject != nullptr ? renderTargetObject->getFramebuffer() : nullptr;
                        imageValue = framebuffer != nullptr ? framebuffer->getResultImage() : nullptr;
                    }
                    if (imageValue == nullptr)
                    {
                        const Texture* defaultTexture = getRenderSubsystemObject()->getParent()->getDefaultTexture();
                        const Texture_RenderAPIObject_Vulkan* defaultTextureObject = defaultTexture != nullptr ? defaultTexture->getRenderAPIObject<Texture_RenderAPIObject_Vulkan>() : nullptr;
                        imageValue = defaultTextureObject != nullptr ? defaultTextureObject->getImage() : nullptr;
                        if (imageValue == nullptr)
                        {
                            JUMA_LOG(error, JSTR("Invalid default texture"));
                            continue;
                        }
                    }
                    VkImage vulkanImageValue = imageValue->get();
                    if (vulkanImageValue == nullptr)
                    {
                        JUMA_LOG(error, JSTR("Invalid image"));
                        continue;
                    }

                    UniformValue_Image& uniformValue = m_UniformValues_Image[uniform.key];
                    if (uniformValue.value == vulkanImageValue)
                    {
                        // No need to update
                        continue;
                    }
                    uniformValue.value = vulkanImageValue;

                    VkDescriptorImageInfo& imageInfo = imageInfos.addDefault();
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = imageValue->getImageView();
                    imageInfo.sampler = imageValue->getSampler();
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &imageInfo;
                }
                break;

            default: 
                JUMA_LOG(warning, JSTR("Unsupported uniform type"));
                continue;
            }

            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSet;
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
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::createDescriptorSet()
    {
        if (m_DescriptorSet != nullptr)
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
        const VkResult result = vkAllocateDescriptorSets(getRenderSubsystemObject()->getDevice(), &allocateInfo, &m_DescriptorSet);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to allocate descriptor set"), result);
            return false;
        }
        return true;
    }

    void Material_RenderAPIObject_Vulkan::clearVulkanData()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();

        for (const auto& pipeline : m_RenderPipelines)
        {
            vkDestroyPipeline(device, pipeline.value, nullptr);
        }
        m_RenderPipelines.clear();

        for (auto& value : m_UniformValues_Buffer_Mat4)
        {
            if (value.value.buffer != nullptr)
            {
                delete value.value.buffer;
                value.value.buffer = nullptr;
            }
        }
        m_UniformValues_Buffer_Mat4.clear();
        m_UniformValues_Image.clear();

        if (m_DescriptorPool != nullptr)
        {
            m_DescriptorSet = nullptr;
            vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
            m_DescriptorPool = nullptr;
        }
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
        if (!bindRenderPipeline(commandBuffer->get(), vertexBuffer->getVertexName(), options->renderPass) ||
            !bindDescriptorSet(commandBuffer->get()))
        {
            return false;
        }
        return vertexBuffer->render(renderOptions);
    }
    bool Material_RenderAPIObject_Vulkan::bindRenderPipeline(VkCommandBuffer commandBuffer, const jstringID& vertexName, 
        const VulkanRenderPass* renderPass)
    {
        VkPipeline pipeline;
        if (!getRenderPipeline(vertexName, renderPass, pipeline))
        {
            return false;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        return true;
    }
    bool Material_RenderAPIObject_Vulkan::getRenderPipeline(const jstringID& vertexName, const VulkanRenderPass* renderPass, 
        VkPipeline& outPipeline)
    {
        const render_pass_id_type renderPassID = renderPass != nullptr ? renderPass->getTypeID() : INVALID_RENDER_PASS_TYPE_ID;
        if (renderPassID == INVALID_RENDER_PASS_TYPE_ID)
        {
            JUMA_LOG(error, JSTR("Invaid render pass ID"));
            return false;
        }

        const RenderPipelineID pipelineID = { vertexName, renderPassID };
        VkPipeline* pipelinePtr = m_RenderPipelines.find(pipelineID);
        if (pipelinePtr != nullptr)
        {
            outPipeline = *pipelinePtr;
            return true;
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

        m_RenderPipelines[pipelineID] = renderPipeline;
        outPipeline = renderPipeline;
        return true;
    }
}

#endif
