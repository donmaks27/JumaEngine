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
        
    private:

        struct UniformValue_Buffer_Mat4
        {
            VulkanBuffer* buffer = nullptr;
            math::matrix4 value = math::matrix4(1);
        };
        struct UniformValue_Image
        {
            VkImage value = nullptr;
        };

        struct RenderPipelineID
        {
            jstringID vertexName = jstringID_NONE;
            render_pass_id_type renderPassID = INVALID_RENDER_PASS_TYPE_ID;

            bool operator<(const RenderPipelineID& ID) const { return (vertexName < ID.vertexName) || ((vertexName == ID.vertexName) && (renderPassID < ID.renderPassID)); }
        };

        VkDescriptorPool m_DescriptorPool = nullptr;
        VkDescriptorSet m_DescriptorSet = nullptr;

        jmap<jstringID, UniformValue_Buffer_Mat4> m_UniformValues_Buffer_Mat4;
        jmap<jstringID, UniformValue_Image> m_UniformValues_Image;

        jmap<RenderPipelineID, VkPipeline> m_RenderPipelines;


        bool createDescriptorPool();

        bool bindDescriptorSet(VkCommandBuffer commandBuffer);
        bool updateDescriptorSet();
        bool createDescriptorSet();

        void clearVulkanData();

        bool bindRenderPipeline(VkCommandBuffer commandBuffer, const jstringID& vertexName, const VulkanRenderPass* renderPass);
        bool getRenderPipeline(const jstringID& vertexName, const VulkanRenderPass* renderPass, VkPipeline& outPipeline);
    };
}

#endif
