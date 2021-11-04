// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Shader_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <fstream>
#include "RenderSubsystem_Vulkan.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    Shader_Vulkan::~Shader_Vulkan()
    {
        if (isValid())
        {
            onShaderPreClear();
            clearVulkanData();
        }
    }

    bool Shader_Vulkan::initInternal(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms)
    {
        createShaderModules(shaderName);
        if (!createDescriptorSetLayout(uniforms) || !createPipelineLayout())
        {
            clearVulkanData();
            return false;
        }
        return true;
    }

    void Shader_Vulkan::createShaderModules(const jstring& shaderName)
    {
        createShaderModule(shaderName + JSTR(".vert.spv"), ShaderStage::Vertex);
        createShaderModule(shaderName + JSTR(".geom.spv"), ShaderStage::Geometry);
        createShaderModule(shaderName + JSTR(".frag.spv"), ShaderStage::Fragment);
    }
    void Shader_Vulkan::createShaderModule(const jstring& fileName, const ShaderStage stage)
    {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            return;
        }

        const size_t fileSize = file.tellg();
        jarray<char> data(fileSize);
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
            JUMA_LOG(warning, JSTR("Failed to create shader module. Code ") + TO_JSTR(result));
            return;
        }

        m_ShaderModules[stage] = shaderModule;
    }

    bool Shader_Vulkan::createDescriptorSetLayout(const jmap<jstring, ShaderUniform>& uniforms)
    {
        jarray<VkDescriptorSetLayoutBinding> layoutBindings;
        for (const auto& uniform : uniforms)
        {
            const ShaderUniform& shaderUniform = uniform.second;

            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = shaderUniform.location;
            layoutBinding.pImmutableSamplers = nullptr;

            for (const auto& stage : shaderUniform.stages)
            {
                switch (stage)
                {
                case ShaderStage::Vertex: layoutBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderStage::Geometry: layoutBinding.stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT; break;
                case ShaderStage::Fragment: layoutBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT; break;
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
            case ShaderUniformType::Image:
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
            JUMA_LOG(error, JSTR("Failed to create descriptor set layout. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }
    bool Shader_Vulkan::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        const VkResult result = vkCreatePipelineLayout(getRenderSubsystem()->getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create pipeline layout. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }

    void Shader_Vulkan::clearVulkanData()
    {
        VkDevice device = getRenderSubsystem()->getDevice();

        if (m_DescriptorSetLayout != nullptr)
        {
            vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
        }
        for (const auto& shaderModule : m_ShaderModules)
        {
            vkDestroyShaderModule(device, shaderModule.second, nullptr);
        }
    }

    jarray<VkPipelineShaderStageCreateInfo> Shader_Vulkan::createPipelineStagesInfo() const
    {
        if (!isValid())
        {
            return {};
        }

        jarray<VkPipelineShaderStageCreateInfo> result;
        for (auto& shaderStageAndModule : m_ShaderModules)
        {
            VkPipelineShaderStageCreateInfo& vertShaderStageInfo = result.addDefault();
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.module = shaderStageAndModule.second;
            vertShaderStageInfo.pName = "main";
            switch (shaderStageAndModule.first)
            {
            case ShaderStage::Vertex: vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; break;
            case ShaderStage::Geometry: vertShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            case ShaderStage::Fragment: vertShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            default: ;
            }
        }
        return result;
    }
}

#endif
