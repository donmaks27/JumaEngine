// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "ShaderOld_Vulkan.h"
#include "subsystems/render/ShaderOld.h"
#include "jutils/jlog.h"
#include "vulkanObjects/VulkanSwapchain.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "ImageOld_Vulkan.h"
#include "RenderOptionsData_Vulkan.h"
#include "vulkanObjects/VulkanCommandBuffer.h"

namespace JumaEngine
{
    struct MaterialUniformData_Vulkan_Buffer
    {
        struct SwapchainImageData
        {
            bool valueActual = false;
            jshared_ptr<VulkanBuffer> buffer = nullptr;
        };
        jarray<SwapchainImageData> data;
    };
    struct MaterialUniformData_Vulkan_Image
    {
        struct SwapchainImageData
        {
            bool valueActual = false;
            jshared_ptr<ImageOld> image = nullptr;
        };
        jarray<SwapchainImageData> data;
    };

    Material_Vulkan::~Material_Vulkan()
    {
        if (isValid())
        {
            onMaterialPreClear();
            clearVulkanData();
        }
    }

    bool Material_Vulkan::initInternal(const jshared_ptr<ShaderOld>& shader)
    {
        if (!createDescriptorPool(shader) || !createMaterialUniformData(shader) || !createDescriptorSets(shader))
        {
            clearVulkanData();
            return false;
        }
        return true;
    }
    bool Material_Vulkan::createDescriptorPool(const jshared_ptr<ShaderOld>& shader)
    {
        VulkanSwapchain* swapchain = getRenderSubsystem()->getSwapchain();
        const uint32 imageCount = (swapchain != nullptr) && swapchain->isValid() ? swapchain->getImageCount() : 0;
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
            switch (uniformNameAndType.value.type)
            {
            case ShaderUniformType::Mat4: type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
            case ShaderUniformType::Texture: type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
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
            JUMA_VULKAN_ERROR_LOG(JSTR("create descriptor pool"), result);
            return false;
        }
        return true;
    }

    bool Material_Vulkan::createMaterialUniformData(const jshared_ptr<ShaderOld>& shader)
    {
        const uint32 imageCount = getRenderSubsystem()->getSwapchain()->getImageCount();
        for (const auto& uniformNameAndType : shader->getUniforms())
        {
            MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.key);
            if (uniformValue == nullptr)
            {
                return false;
            }

            switch (uniformNameAndType.value.type)
            {
            case ShaderUniformType::Mat4:
                createMaterialUniformData_Matrix4(uniformValue, imageCount);
                break;

            case ShaderUniformType::Texture:
                createMaterialUniformData_Image(uniformValue, imageCount);
                break;

            default: ;
            }
        }
        return true;
    }
    void Material_Vulkan::createMaterialUniformData_Matrix4(MaterialUniform* uniformValue, const uint32 imageCount)
    {
        MaterialUniformData_Vulkan_Buffer* uniformData;
        if (uniformValue->data == nullptr)
        {
            uniformValue->data = uniformData = new MaterialUniformData_Vulkan_Buffer();
        }
        else
        {
            uniformData = static_cast<MaterialUniformData_Vulkan_Buffer*>(uniformValue->data);
        }

        const int32 size = uniformData->data.getSize();
        if (size != static_cast<int32>(imageCount))
        {
            uniformData->data.resize(imageCount);
            for (int32 index = size; index < static_cast<int32>(imageCount); index++)
            {
                MaterialUniformData_Vulkan_Buffer::SwapchainImageData& data = uniformData->data[index];
                data.buffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
                data.buffer->init(
                    sizeof(math::matrix4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {},
                    VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                );
            }
        }
    }
    void Material_Vulkan::createMaterialUniformData_Image(MaterialUniform* uniformValue, const uint32 imageCount)
    {
        MaterialUniformData_Vulkan_Image* uniformData;
        if (uniformValue->data == nullptr)
        {
            uniformValue->data = uniformData = new MaterialUniformData_Vulkan_Image();
        }
        else
        {
            uniformData = static_cast<MaterialUniformData_Vulkan_Image*>(uniformValue->data);
        }

        if (uniformData->data.getSize() != static_cast<int32>(imageCount))
        {
            uniformData->data.resize(imageCount);
        }
    }

    bool Material_Vulkan::createDescriptorSets(const jshared_ptr<ShaderOld>& shader)
    {
        if (m_DescriptorPool == nullptr)
        {
            return true;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        const ShaderOld_Vulkan* shaderVulkan = dynamic_cast<const ShaderOld_Vulkan*>(shader.get());
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
            JUMA_VULKAN_ERROR_LOG(JSTR("create descriptor sets"), result);
            return false;
        }
        
        jarray<VkDescriptorBufferInfo> bufferInfos;
        jarray<VkDescriptorImageInfo> imageInfos;
        jarray<VkWriteDescriptorSet> descriptorWrites;
        const jmap<jstring, ShaderUniform>& uniforms = shader->getUniforms();
        {
            const int32 uniformCount = uniforms.getSize();
            bufferInfos.reserve(uniformCount);
            imageInfos.reserve(uniformCount);
            descriptorWrites.reserve(uniformCount);
        }

        for (int32 imageIndex = 0; imageIndex < static_cast<int32>(imageCount); imageIndex++)
        {
            bufferInfos.clear();
            imageInfos.clear();
            descriptorWrites.clear();

            for (const auto& uniformNameAndType : uniforms)
            {
                VkWriteDescriptorSet descriptorWrite{};
                MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.key);
                if (uniformValue == nullptr)
                {
                    continue;
                }

                switch (uniformNameAndType.value.type)
                {
                case ShaderUniformType::Mat4:
                    {
                        MaterialUniformData_Vulkan_Buffer* uniformData = static_cast<MaterialUniformData_Vulkan_Buffer*>(uniformValue->data);
                        if ((uniformData == nullptr) || !uniformData->data.isValidIndex(imageIndex))
                        {
                            // Data should be OK here, something wrong
                            return false;
                        }

                        const jshared_ptr<VulkanBuffer>& buffer = uniformData->data[imageIndex].buffer;
                        VkDescriptorBufferInfo& bufferInfo = bufferInfos.addDefault();
                        bufferInfo.buffer = buffer->get();
                        bufferInfo.offset = 0;
                        bufferInfo.range = buffer->getSize();

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        descriptorWrite.descriptorCount = 1;
                        descriptorWrite.pBufferInfo = &bufferInfo;
                    }
                    break;

                case ShaderUniformType::Texture:
                    {
                        MaterialUniformData_Vulkan_Image* uniformData = static_cast<MaterialUniformData_Vulkan_Image*>(uniformValue->data);
                        if ((uniformData == nullptr) || !uniformData->data.isValidIndex(imageIndex))
                        {
                            // Data should be OK here, something wrong
                            return false;
                        }

                        ImageOld_Vulkan* image = dynamic_cast<ImageOld_Vulkan*>(uniformData->data[imageIndex].image.get());
                        if (image == nullptr)
                        {
                            continue;
                        }

                        VkDescriptorImageInfo& imageInfo = imageInfos.addDefault();
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageInfo.imageView = image->getImageView();
                        imageInfo.sampler = image->getSampler();

                        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        descriptorWrite.descriptorCount = 1;
                        descriptorWrite.pImageInfo = &imageInfo;
                    }
                    break;

                default: return false;
                }
                
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = m_DescriptorSets[imageIndex];
                descriptorWrite.dstBinding = uniformNameAndType.value.location;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrites.add(descriptorWrite);
            }
            vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.getSize()), descriptorWrites.getData(), 0, nullptr);
        }
        return true;
    }

    void Material_Vulkan::clearVulkanData(const jshared_ptr<ShaderOld>& shader)
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
                MaterialUniform* uniformValue = getUniformValue(uniformNameAndType.key);
                switch (uniformNameAndType.value.type)
                {
                case ShaderUniformType::Mat4:
                    delete static_cast<MaterialUniformData_Vulkan_Buffer*>(uniformValue->data);
                    break;

                case ShaderUniformType::Texture: 
                    delete static_cast<MaterialUniformData_Vulkan_Image*>(uniformValue->data);
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
                MaterialUniformData_Vulkan_Buffer* uniformData = static_cast<MaterialUniformData_Vulkan_Buffer*>(uniform->data);
                for (auto& dataBuffer : uniformData->data)
                {
                    dataBuffer.valueActual = false;
                }
            }
            break;

        case ShaderUniformType::Texture: 
            {
                MaterialUniformData_Vulkan_Image* uniformData = static_cast<MaterialUniformData_Vulkan_Image*>(uniform->data);
                for (auto& dataImage : uniformData->data)
                {
                    dataImage.valueActual = false;
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
            MaterialUniform* const& uniformValue = m_UniformValues[uniformIndex];
            if (uniformValue->data == nullptr)
            {
                continue;
            }
            switch (uniformValue->type)
            {
            case ShaderUniformType::Mat4:
                updateMaterialUniformData_Matrix4(uniformValue, data.swapchainImageIndex);
                break;

            case ShaderUniformType::Texture:
                updateMaterialUniformData_Image(uniformIndex, uniformValue, data.swapchainImageIndex);
                break;

            default: ;
            }
        }

        const ShaderOld_Vulkan* shader = dynamic_cast<const ShaderOld_Vulkan*>(getShader().get());
        vkCmdBindDescriptorSets(
            data.commandBuffer->get(), VK_PIPELINE_BIND_POINT_GRAPHICS, 
            shader->getPipelineLayout(), 0, 1, &m_DescriptorSets[data.swapchainImageIndex], 
            0, nullptr
        );
        return true;
    }
    void Material_Vulkan::updateMaterialUniformData_Matrix4(MaterialUniform* uniformValue, const uint32 imageIndex)
    {
        MaterialUniformData_Vulkan_Buffer* uniformData = static_cast<MaterialUniformData_Vulkan_Buffer*>(uniformValue->data);
        if (!uniformData->data.isValidIndex(imageIndex))
        {
            return;
        }

        MaterialUniformData_Vulkan_Buffer::SwapchainImageData& imageData = uniformData->data[imageIndex];
        if (imageData.valueActual)
        {
            return;
        }

        math::matrix4 value;
        MaterialUniformActions::get<ShaderUniformType::Mat4>(uniformValue, value);
        imageData.buffer->setData(&value[0][0]);
        imageData.valueActual = true;
    }
    void Material_Vulkan::updateMaterialUniformData_Image(const int32 uniformIndex, MaterialUniform* uniformValue, const uint32 imageIndex)
    {
        MaterialUniformData_Vulkan_Image* uniformData = static_cast<MaterialUniformData_Vulkan_Image*>(uniformValue->data);
        if (!uniformData->data.isValidIndex(imageIndex))
        {
            return;
        }

        MaterialUniformData_Vulkan_Image::SwapchainImageData& imageData = uniformData->data[imageIndex];
        if (imageData.valueActual)
        {
            return;
        }

        MaterialUniformActions::get<ShaderUniformType::Texture>(uniformValue, imageData.image);
        if (imageData.image == nullptr)
        {
            // TODO: Deal with this case later
            imageData.valueActual = true;
            return;
        }

        ImageOld_Vulkan* imageVulkan = dynamic_cast<ImageOld_Vulkan*>(imageData.image.get());
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = imageVulkan->getImageView();
        imageInfo.sampler = imageVulkan->getSampler();

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[imageIndex];
        descriptorWrite.dstBinding = getShader()->getUniforms()[m_UniformNames[uniformIndex]].location;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;
        vkUpdateDescriptorSets(getRenderSubsystem()->getDevice(), 1, &descriptorWrite, 0, nullptr);

        imageData.valueActual = true;
    }
}

#endif
