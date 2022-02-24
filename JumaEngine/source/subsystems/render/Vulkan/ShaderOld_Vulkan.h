// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/ShaderOld.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class ShaderOld_Vulkan final : public ShaderOld, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(ShaderOld_Vulkan, ShaderOld)

    public:
        ShaderOld_Vulkan() = default;
        virtual ~ShaderOld_Vulkan() override;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }
        VkPipelineLayout getPipelineLayout() const { return m_PipelineLayout; }

        const jarray<VkPipelineShaderStageCreateInfo>& getPipelineStageInfos() const { return m_PipelineStageInfos; }

    protected:

        virtual bool initInternal(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms) override;
        virtual void clearInternal() override { clearVulkanData(); }

    private:

        jmap<ShaderStage, VkShaderModule> m_ShaderModules;
        VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;
        jarray<VkPipelineShaderStageCreateInfo> m_PipelineStageInfos;


        void createShaderModules(const jstring& shaderName);
        void createShaderModule(const jstring& fileName, ShaderStage stage);
        bool createDescriptorSetLayout(const jmap<jstring, ShaderUniform>& uniforms);
        bool createPipelineLayout();
        void createPipelineStageInfos();

        void clearVulkanData();
    };
}

#endif
