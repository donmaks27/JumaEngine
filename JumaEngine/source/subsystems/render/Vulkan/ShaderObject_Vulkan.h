// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Shader.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"

namespace JumaEngine
{
    class ShaderObject_Vulkan : public ShaderObject, public VulkanContextObjectBase
    {
    public:
        ShaderObject_Vulkan() = default;
        virtual ~ShaderObject_Vulkan() override;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }
        VkPipelineLayout getPipelineLayout() const { return m_PipelineLayout; }
        const jarray<VkPipelineShaderStageCreateInfo>& getPipelineStageInfos() const { return m_PipelineStageInfos; }

    protected:

        virtual bool initInternal() override;
        virtual void clearInternal() override { clearVulkanData(); }

    private:

        jmap<ShaderStage, VkShaderModule> m_ShaderModules;
        VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;

        jarray<VkPipelineShaderStageCreateInfo> m_PipelineStageInfos;


        bool createShaderModule(const jstring& fileName, ShaderStage stage, bool optional = false);
        bool createDescriptorSetLayout(const jmap<jstringID, ShaderUniform>& uniforms);
        bool createPipelineLayout();
        void cachePipelineStageInfos();

        void clearVulkanData();
    };
}

#endif
