﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Shader.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class Shader_Vulkan final : public Shader, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(Shader_Vulkan, Shader)

    public:
        Shader_Vulkan() = default;
        virtual ~Shader_Vulkan() override;

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
