// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Material.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "vulkanObjects/VulkanRenderPassDescription.h"

namespace JumaEngine
{
    class VulkanImage;
    class VulkanRenderPass;
    class VulkanBuffer;

    class MaterialRenderAPIObject_Vulkan : public MaterialRenderAPIObject, public VulkanContextObjectBase
    {
    public:
        MaterialRenderAPIObject_Vulkan() = default;
        virtual ~MaterialRenderAPIObject_Vulkan() override;

        virtual bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;
        
        virtual void onMaterialParamChanged(const jstringID& paramName) override;

    private:

        struct DescriptorSetContainer
        {
            VkDescriptorSet descriptorSet = nullptr;
            bool valid = false;
        };
        struct UniformValue_Buffer
        {
            bool valid = false;
            VulkanBuffer* buffer = nullptr;
        };
        struct UniformValue_Image
        {
            bool valid = false;
            VulkanImage* image = nullptr;
        };

        VkDescriptorPool m_DescriptorPool = nullptr;
        jarray<DescriptorSetContainer> m_DescriptorSets;

        jmap<jstringID, jarray<UniformValue_Buffer>> m_UniformValues_Buffer;
        jmap<jstringID, jarray<UniformValue_Image>> m_UniformValues_Image;

        jmap<jstringID, jmap<render_pass_id_type, VkPipeline>> m_RenderPipelines;


        bool createDescriptorPool();

        bool bindDescriptorSet(VkCommandBuffer commandBuffer, int8 frameIndex);
        bool updateDescriptorSet(int8 frameIndex);
        bool createDescriptorSet(int8 frameIndex);

        template<ShaderUniformType Type>
        bool updateBufferUniformValue(const jstringID& name, uint32 frameIndex, VkDescriptorBufferInfo& outInfo);
        template<ShaderUniformType Type>
        bool updateBufferUniformValueData(const jstringID& name, uint32 frameIndex, VkDescriptorBufferInfo& outInfo);
        bool updateBufferUniformValueData(UniformValue_Buffer* bufferValue, uint64 dataSize, const void* data, VkDescriptorBufferInfo& outInfo);

        bool updateTextureUniformValue(const jstringID& name, int8 frameIndex, VkDescriptorImageInfo& outInfo);
        bool updateRenderTargetUniformValue(const jstringID& name, int8 frameIndex, VkDescriptorImageInfo& outInfo);

        void clearVulkanData();

        bool bindRenderPipeline(VkCommandBuffer commandBuffer, const jstringID& vertexName, const VulkanRenderPass* renderPass);
        bool createRenderPipeline(const jstringID& vertexName, const VulkanRenderPass* renderPass, VkPipeline& outPipeline);
    };

    template<ShaderUniformType Type>
    bool MaterialRenderAPIObject_Vulkan::updateBufferUniformValue(const jstringID& name, const uint32 frameIndex, VkDescriptorBufferInfo& outInfo)
    {
        if (m_Parent->isOverrideParam(name))
        {
            return updateBufferUniformValueData<Type>(name, frameIndex, outInfo);
        }
        if (m_Parent->isMaterialInstance())
        {
            MaterialRenderAPIObject_Vulkan* baseMaterialObject = dynamic_cast<MaterialRenderAPIObject_Vulkan*>(m_Parent->getBaseMaterial()->getRenderAPIObject());
            return baseMaterialObject->updateBufferUniformValue<Type>(name, frameIndex, outInfo);
        }
        return false;
    }
    template<ShaderUniformType Type>
    bool MaterialRenderAPIObject_Vulkan::updateBufferUniformValueData(const jstringID& name, const uint32 frameIndex, VkDescriptorBufferInfo& outInfo)
    {
        jarray<UniformValue_Buffer>* buffers = m_UniformValues_Buffer.find(name);
        UniformValue_Buffer* bufferValue = buffers != nullptr ? buffers->findByIndex(static_cast<int32>(frameIndex)) : nullptr;
        if (bufferValue == nullptr)
        {
            JUMA_LOG(error, JSTR("Can't find uniform value buffer, this shouldn't happen"));
            return false;
        }

        if (!bufferValue->valid)
        {
            typename MaterialUniformInfo<Type>::value_type value;
            if (m_Parent->getParamValue<Type>(name, value) && this->updateBufferUniformValueData(bufferValue, sizeof(value), &value, outInfo))
            {
                bufferValue->valid = true;
                return true;
            }
        }
        return false;
    }
}

#endif
