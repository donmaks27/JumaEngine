// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "Shader_Vulkan.h"
#include "subsystems/render/Shader.h"
#include "utils/jlog.h"
#include "vulkanObjects/VulkanSwapchain.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "Image_Vulkan.h"
#include "RenderOptionsData_Vulkan.h"
#include "vulkanObjects/VulkanCommandBuffer.h"

namespace JumaEngine
{
    struct MaterialUniformData_Vulkan
    {
        bool valueActual = false;
    };
    struct MaterialUniformData_Vulkan_Buffer : public MaterialUniformData_Vulkan
    {
        jshared_ptr<VulkanBuffer> buffer = nullptr;
    };
    struct MaterialUniformData_Vulkan_Image : public MaterialUniformData_Vulkan
    {
        jshared_ptr<Image> image = nullptr;
    };

    Material_Vulkan::~Material_Vulkan()
    {
        if (isValid())
        {
            onMaterialPreClear();
            clearVulkanData();
        }
    }

    bool Material_Vulkan::initInternal(const jshared_ptr<Shader>& shader)
    {
        if (!createDescriptorPool(shader) || !createMaterialUniformDatas(shader) || !createDescriptorSets(shader))
        {
            clearVulkanData();
            return false;
        }
        return true;
    }
    bool Material_Vulkan::createDescriptorPool(const jshared_ptr<Shader>& shader)
    {
        const jshared_ptr<VulkanSwapchain>& swapchain = getRenderSubsystem()->getSwapchain();
        const uint32 imageCount = (swapchain != nullptr) || swapchain->isValid() ? swapchain->getImageCount() : 0;
        if (imageCount == 0)
        {
            return false;
        }

        const jmap<jstring, ShaderUniform>& uniforms = shader->getUniforms();
        if (uniforms.getSize() == 0)
        {
            return true;
        }

        jarray<VkDescriptorPoolSize> poolSizes;
        poolSizes.reserve(imageCount);
        for (const auto& uniformNameAndType : uniforms)
        {
            VkDescriptorType type;
            switch (uniformNameAndType.second.type)
            {
            case ShaderUniformType::Mat4: type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
            case ShaderUniformType::Image: type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
            default: return false;
            }

            VkDescriptorPoolSize& poolSize = poolSizes.addDefault();
            poolSize.type = type;
            poolSize.descriptorCount = imageCount;
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32>(poolSizes.getSize());
        poolInfo.pPoolSizes = poolSizes.getData();
        poolInfo.maxSets = static_cast<uint32>(imageCount);
        const VkResult result = vkCreateDescriptorPool(getRenderSubsystem()->getDevice(), &poolInfo, nullptr, &m_DescriptorPool);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create descriptor pool. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }
    bool Material_Vulkan::createMaterialUniformDatas(const jshared_ptr<Shader>& shader)
    {
        const uint32 imageCount = getRenderSubsystem()->getSwapchain()->getImageCount();
        for (const auto& uniformNameAndType : shader->getUniforms())
        {
            MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.first);
            if (uniformValue == nullptr)
            {
                return false;
            }

            switch (uniformNameAndType.second.type)
            {
            case ShaderUniformType::Mat4:
                {
                    jarray<MaterialUniformData_Vulkan_Buffer>* data = static_cast<jarray<MaterialUniformData_Vulkan_Buffer>*>(uniformValue->data);
                    if (data == nullptr)
                    {
                        uniformValue->data = data = new jarray<MaterialUniformData_Vulkan_Buffer>();
                    }
                    if (data->getSize() != static_cast<int32>(imageCount))
                    {
                        const uint32 oldSize = static_cast<uint32>(data->getSize());
                        data->resize(imageCount);
                        for (uint32 index = oldSize; index < imageCount; index++)
                        {
                            MaterialUniformData_Vulkan_Buffer& dataBuffer = data->get(index);
                            dataBuffer.buffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
                            dataBuffer.buffer->init(
                                sizeof(math::matrix4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {},
                                VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                            );
                        }
                    }
                }
                break;

            case ShaderUniformType::Image:
                {
                    jarray<MaterialUniformData_Vulkan_Image>* data = static_cast<jarray<MaterialUniformData_Vulkan_Image>*>(uniformValue->data);
                    if (data == nullptr)
                    {
                        uniformValue->data = data = new jarray<MaterialUniformData_Vulkan_Image>();
                    }
                    data->resize(imageCount);
                }
                break;

            default: return false;
            }
        }
        return true;
    }
    bool Material_Vulkan::createDescriptorSets(const jshared_ptr<Shader>& shader)
    {
        if (m_DescriptorPool == nullptr)
        {
            return true;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        const jshared_ptr<Shader_Vulkan> shaderVulkan = jshared_dynamic_cast<Shader_Vulkan>(shader);
        const uint32 imageCount = getRenderSubsystem()->getSwapchain()->getImageCount();

        m_DescriptorSets.resize(imageCount);

        const jarray<VkDescriptorSetLayout> layouts(imageCount, shaderVulkan->getDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = m_DescriptorPool;
        allocateInfo.descriptorSetCount = static_cast<uint32>(layouts.getSize());
        allocateInfo.pSetLayouts = layouts.getData();
        const VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, m_DescriptorSets.getData());
        if (result != VK_SUCCESS)
        {
            m_DescriptorSets.clear();
            JUMA_LOG(error, JSTR("Failed to create descriptor sets. Code ") + TO_JSTR(result));
            return false;
        }
        
        jarray<VkDescriptorBufferInfo> bufferInfos;
        jarray<VkDescriptorImageInfo> imageInfos;
        jarray<VkWriteDescriptorSet> descriptorWrites;
        const jmap<jstring, ShaderUniform>& uniforms = shader->getUniforms();
        for (int32 descriptorSetIndex = 0; descriptorSetIndex < m_DescriptorSets.getSize(); descriptorSetIndex++)
        {
            bufferInfos.reserve(uniforms.getSize());
            imageInfos.reserve(uniforms.getSize());
            descriptorWrites.reserve(uniforms.getSize());
            for (const auto& uniformNameAndType : uniforms)
            {
                VkWriteDescriptorSet descriptorWrite{};
                MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.first);
                switch (uniformNameAndType.second.type)
                {
                case ShaderUniformType::Mat4:
                    {
                        jarray<MaterialUniformData_Vulkan_Buffer>* data = uniformValue != nullptr ? static_cast<jarray<MaterialUniformData_Vulkan_Buffer>*>(uniformValue->data) : nullptr;
                        if (data == nullptr)
                        {
                            return false;
                        }

                        const jshared_ptr<VulkanBuffer> buffer = data->get(descriptorSetIndex).buffer;
                        VkDescriptorBufferInfo& bufferInfo = bufferInfos.addDefault();
                        bufferInfo.buffer = buffer->get();
                        bufferInfo.offset = 0;
                        bufferInfo.range = buffer->getSize();

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorWrite.descriptorCount = 1;
                        descriptorWrite.pBufferInfo = &bufferInfo;
                    }
                    break;

                case ShaderUniformType::Image:
                    {
                        jarray<MaterialUniformData_Vulkan_Image>* data = uniformValue != nullptr ? static_cast<jarray<MaterialUniformData_Vulkan_Image>*>(uniformValue->data) : nullptr;
                        if (data == nullptr)
                        {
                            return false;
                        }

                        jshared_ptr<Image_Vulkan> imageVulkan = jshared_dynamic_cast<Image_Vulkan>(data->get(descriptorSetIndex).image);
                        if (imageVulkan == nullptr)
                        {
                            continue;
                        }
                        VkDescriptorImageInfo& imageInfo = imageInfos.addDefault();
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = imageVulkan->getImageView();
                        imageInfo.sampler = imageVulkan->getSampler();

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.descriptorCount = 1;
                        descriptorWrite.pImageInfo = &imageInfo;
                    }
                    break;

                default: return false;
                }
                
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_DescriptorSets[descriptorSetIndex];
                descriptorWrite.dstBinding = uniformNameAndType.second.location;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrites.add(descriptorWrite);
            }
            vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.getSize()), descriptorWrites.getData(), 0, nullptr);
            bufferInfos.clear();
            imageInfos.clear();
            descriptorWrites.clear();
        }
        return true;
    }

    void Material_Vulkan::clearVulkanData(const jshared_ptr<Shader>& shader)
    {
        m_DescriptorSets.clear();
        if (m_DescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(getRenderSubsystem()->getDevice(), m_DescriptorPool, nullptr);
        }

        if (shader != nullptr)
        {
            for (const auto& uniformNameAndType : shader->getUniforms())
            {
                MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.first);
                switch (uniformNameAndType.second.type)
                {
                case ShaderUniformType::Mat4:
                    delete static_cast<jarray<MaterialUniformData_Vulkan_Buffer>*>(uniformValue->data);
                    break;

                case ShaderUniformType::Image: 
                    delete static_cast<jarray<MaterialUniformData_Vulkan_Image>*>(uniformValue->data);
                    break;

                default: ;
                }
                uniformValue->data = nullptr;
            }
        }
    }

    void Material_Vulkan::onMaterialUniformChanged(const jstring& name)
    {
        MaterialUniform* uniform = getUniformValue(name);
        if (uniform->data == nullptr)
        {
            return;
        }

        switch (uniform->type)
        {
        case ShaderUniformType::Mat4:
            {
                for (auto& dataBuffer : *static_cast<jarray<MaterialUniformData_Vulkan_Buffer>*>(uniform->data))
                {
                    dataBuffer.valueActual = false;
                }
            }
            break;

        case ShaderUniformType::Image: 
            {
                for (auto& dataBuffer : *static_cast<jarray<MaterialUniformData_Vulkan_Image>*>(uniform->data))
                {
                    dataBuffer.valueActual = false;
                }
            }
            break;

        default: ;
        }
    }

    bool Material_Vulkan::render(const RenderOptionsData_Vulkan& data)
    {
        if (!isValid())
        {
            return false;
        }
        if (m_DescriptorPool == nullptr)
        {
            return true;
        }
        if (!m_DescriptorSets.isValidIndex(data.swapchainImageIndex))
        {
            return false;
        }

        for (int32 uniformIndex = 0; uniformIndex < m_UniformValues.getSize(); uniformIndex++)
        {
            MaterialUniform* const& uniform = m_UniformValues[uniformIndex];
            switch (uniform->type)
            {
            case ShaderUniformType::Mat4:
                {
                    jarray<MaterialUniformData_Vulkan_Buffer>* uniformData = static_cast<jarray<MaterialUniformData_Vulkan_Buffer>*>(uniform->data);
                    MaterialUniformData_Vulkan_Buffer& dataBuffer = uniformData->get(data.swapchainImageIndex);
                    if (!dataBuffer.valueActual)
                    {
                        math::matrix4 value;
                        MaterialUniformActions::get<ShaderUniformType::Mat4>(uniform, value);
                        dataBuffer.buffer->setData(&value[0][0]);
                        dataBuffer.valueActual = true;
                    }
                }
                break;

            case ShaderUniformType::Image:
                {
                    jarray<MaterialUniformData_Vulkan_Image>* uniformData = static_cast<jarray<MaterialUniformData_Vulkan_Image>*>(uniform->data);
                    MaterialUniformData_Vulkan_Image& dataImage= uniformData->get(data.swapchainImageIndex);
                    if (!dataImage.valueActual)
                    {
                        MaterialUniformActions::get<ShaderUniformType::Image>(uniform, dataImage.image);
                        dataImage.valueActual = true;

                        jshared_ptr<Image_Vulkan> imageVulkan = jshared_dynamic_cast<Image_Vulkan>(dataImage.image);
                        if (imageVulkan == nullptr)
                        {
                            continue;
                        }
                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = imageVulkan->getImageView();
                        imageInfo.sampler = imageVulkan->getSampler();

                        VkWriteDescriptorSet descriptorWrite{};
                        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        descriptorWrite.dstSet = m_DescriptorSets[data.swapchainImageIndex];
                        descriptorWrite.dstBinding = getShader()->getUniforms()[m_UniformNames[uniformIndex]].location;
                        descriptorWrite.dstArrayElement = 0;
                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.descriptorCount = 1;
                        descriptorWrite.pImageInfo = &imageInfo;
                        vkUpdateDescriptorSets(getRenderSubsystem()->getDevice(), 1, &descriptorWrite, 0, nullptr);
                    }
                }
                break;

            default: ;
            }
        }

        const Shader_Vulkan* shader = dynamic_cast<const Shader_Vulkan*>(getShader().get());
        vkCmdBindDescriptorSets(
            data.commandBuffer->get(), VK_PIPELINE_BIND_POINT_GRAPHICS, 
            shader->getPipelineLayout(), 0, 1, &m_DescriptorSets[data.swapchainImageIndex], 
            0, nullptr
        );
        return true;
    }
}

#endif
