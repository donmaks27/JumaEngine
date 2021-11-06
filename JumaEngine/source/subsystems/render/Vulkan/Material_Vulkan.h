// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Material.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    struct RenderOptionsData_Vulkan;
    class VulkanCommandBuffer;

    class Material_Vulkan final : public Material, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(Material_Vulkan, Material)

    public:
        Material_Vulkan() = default;
        virtual ~Material_Vulkan() override;

        bool render(const RenderOptionsData_Vulkan& data);

    protected:

        virtual bool initInternal(const jshared_ptr<Shader>& shader) override;
        virtual void clearInternal() override { clearVulkanData(); }

        virtual void onMaterialUniformChanged(const jstring& name) override;

    private:

        VkDescriptorPool m_DescriptorPool = nullptr;
        jarray<VkDescriptorSet> m_DescriptorSets;


        bool createDescriptorPool(const jshared_ptr<Shader>& shader);
        bool createMaterialUniformDatas(const jshared_ptr<Shader>& shader);
        bool createDescriptorSets(const jshared_ptr<Shader>& shader);

        void clearVulkanData() { clearVulkanData(getShader()); }
        void clearVulkanData(const jshared_ptr<Shader>& shader);
    };
}

#endif
