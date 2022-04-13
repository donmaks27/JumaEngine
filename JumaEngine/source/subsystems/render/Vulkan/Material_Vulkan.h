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

    class Material_RenderAPIObject_Vulkan : public Material_RenderAPIObject, public VulkanContextObjectBase
    {
    public:
        Material_RenderAPIObject_Vulkan() = default;
        virtual ~Material_RenderAPIObject_Vulkan() override;

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
        DescriptorSetContainer m_DescriptorSet;

        jmap<jstringID, UniformValue_Buffer> m_UniformValues_Buffer;
        jmap<jstringID, UniformValue_Image> m_UniformValues_Image;

        jmap<jstringID, jmap<render_pass_id_type, VkPipeline>> m_RenderPipelines;


        bool createDescriptorPool();

        bool bindDescriptorSet(VkCommandBuffer commandBuffer);
        bool updateDescriptorSet();
        bool createDescriptorSet();

        template<ShaderUniformType Type>
        bool updateBufferUniformValue(const jstringID& name, VkDescriptorBufferInfo& outInfo);
        template<ShaderUniformType Type>
        bool updateBufferUniformValueData(const jstringID& name, VkDescriptorBufferInfo& outInfo);
        bool updateBufferUniformValueData(UniformValue_Buffer* bufferValue, uint64 dataSize, const void* data, VkDescriptorBufferInfo& outInfo);

        bool updateTextureUniformValue(const jstringID& name, VkDescriptorImageInfo& outInfo);
        bool updateRenderTargetUniformValue(const jstringID& name, VkDescriptorImageInfo& outInfo);

        void clearVulkanData();

        bool bindRenderPipeline(VkCommandBuffer commandBuffer, const jstringID& vertexName, const VulkanRenderPass* renderPass);
        bool createRenderPipeline(const jstringID& vertexName, const VulkanRenderPass* renderPass, VkPipeline& outPipeline);
    };

    template<ShaderUniformType Type>
    bool Material_RenderAPIObject_Vulkan::updateBufferUniformValue(const jstringID& name, VkDescriptorBufferInfo& outInfo)
    {
        if (m_Parent->isOverrideParam(name))
        {
            return updateBufferUniformValueData<Type>(name, outInfo);
        }
        if (m_Parent->isMaterialInstance())
        {
            Material_RenderAPIObject_Vulkan* baseMaterialObject = dynamic_cast<Material_RenderAPIObject_Vulkan*>(m_Parent->getBaseMaterial()->getRenderAPIObject());
            return baseMaterialObject->updateBufferUniformValue<Type>(name, outInfo);
        }
        return false;
    }
    template<ShaderUniformType Type>
    bool Material_RenderAPIObject_Vulkan::updateBufferUniformValueData(const jstringID& name, VkDescriptorBufferInfo& outInfo)
    {
        UniformValue_Buffer* bufferValue = m_UniformValues_Buffer.find(name);
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
