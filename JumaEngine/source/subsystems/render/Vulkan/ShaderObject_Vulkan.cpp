// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "ShaderObject_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <fstream>

#include "RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    ShaderObject_Vulkan::~ShaderObject_Vulkan()
    {
        clearVulkanData();
    }

    bool ShaderObject_Vulkan::initInternal()
    {
        if (!createShaderModule(m_Parent->getVertexShaderName() + JSTR(".vert.spv"), ShaderStage::Vertex) ||
            !createShaderModule(m_Parent->getFragmentShaderName() + JSTR(".frag.spv"), ShaderStage::Fragment))
        {
            clearVulkanData();
            return false;
        }
        if (!createDescriptorSetLayout(m_Parent->getUniforms()) || !createPipelineLayout())
        {
            clearVulkanData();
            return false;
        }

        cachePipelineStageInfos();
        return true;
    }
    bool ShaderObject_Vulkan::createShaderModule(const jstring& fileName, const ShaderStage stage, const bool optional)
    {
        std::ifstream file(*fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            if (!optional)
            {
                JUMA_LOG(error, JSTR("Can't open file ") + fileName);
            }
            return false;
        }

        const size_t fileSize = file.tellg();
        jarray<char> data(static_cast<int32>(fileSize));
        file.seekg(0);
        file.read(data.getData(), fileSize);
        file.close();

        VkShaderModule shaderModule = nullptr;
        VkShaderModuleCreateInfo shaderInfo{};
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.codeSize = data.getSize();
        shaderInfo.pCode = reinterpret_cast<const uint32*>(data.getData());
        const VkResult result = vkCreateShaderModule(getRenderSubsystem()->getDevice(), &shaderInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create shader module ") + fileName, result);
            return false;
        }

        m_ShaderModules[stage] = shaderModule;
        return true;
    }
    bool ShaderObject_Vulkan::createDescriptorSetLayout(const jmap<jstringID, ShaderUniform>& uniforms)
    {
        jarray<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(uniforms.getSize());
        for (const auto& uniform : uniforms)
        {
            const ShaderUniform& shaderUniform = uniform.value;

            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = shaderUniform.location;
            layoutBinding.pImmutableSamplers = nullptr;

            for (const auto& stage : shaderUniform.stages)
            {
                switch (stage)
                {
                case ShaderStage::Vertex: layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderStage::Geometry: layoutBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT; break;
                case ShaderStage::Fragment: layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                default: ;
                }
            }

            switch (shaderUniform.type)
            {
            case ShaderUniformType::Mat4:
                {
                    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    layoutBinding.descriptorCount = 1;
                }
                break;
            case ShaderUniformType::Texture:
                {
                    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    layoutBinding.descriptorCount = 1;
                }
                break;

            default: ;
            }

            layoutBindings.add(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32>(layoutBindings.getSize());
        layoutInfo.pBindings = layoutBindings.getData();
        const VkResult result = vkCreateDescriptorSetLayout(getRenderSubsystem()->getDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create descriptor set layout."), result);
            return false;
        }
        return true;
    }
    bool ShaderObject_Vulkan::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        const VkResult result = vkCreatePipelineLayout(getRenderSubsystem()->getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create pipeline layout."), result);
            return false;
        }
        return true;
    }
    void ShaderObject_Vulkan::cachePipelineStageInfos()
    {
        m_PipelineStageInfos.reserve(m_ShaderModules.getSize());
        m_PipelineStageInfos.clear();
        for (auto& shaderStageAndModule : m_ShaderModules)
        {
            VkPipelineShaderStageCreateInfo& stageInfo = m_PipelineStageInfos.addDefault();
            stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageInfo.module = shaderStageAndModule.value;
            stageInfo.pName = "main";
            switch (shaderStageAndModule.key)
            {
            case ShaderStage::Vertex: stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
            case ShaderStage::Geometry: stageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            case ShaderStage::Fragment: stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default: ;
            }
            stageInfo.flags = 0;
        }
    }

    void ShaderObject_Vulkan::clearVulkanData()
    {
        VkDevice device = getRenderSubsystem()->getDevice();

        m_PipelineStageInfos.clear();
        if (m_PipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
        }
        if (m_DescriptorSetLayout != nullptr)
        {
            vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
        }
        for (const auto& shaderModule : m_ShaderModules)
        {
            vkDestroyShaderModule(device, shaderModule.value, nullptr);
        }
        m_ShaderModules.clear();
    }
}

#endif
