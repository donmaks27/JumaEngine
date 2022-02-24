﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/MaterialOld.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    struct RenderOptionsData_Vulkan;
    class VulkanCommandBuffer;

    class Material_Vulkan final : public MaterialOld, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(Material_Vulkan, MaterialOld)

    public:
        Material_Vulkan() = default;
        virtual ~Material_Vulkan() override;

        bool render(const RenderOptionsData_Vulkan& data);

    protected:

        virtual bool initInternal(const jshared_ptr<ShaderOld>& shader) override;
        virtual void clearInternal() override { clearVulkanData(); }

        virtual void onMaterialUniformChanged(const jstring& name) override;

    private:

        VkDescriptorPool m_DescriptorPool = nullptr;
        jarray<VkDescriptorSet> m_DescriptorSets;


        bool createDescriptorPool(const jshared_ptr<ShaderOld>& shader);

        bool createMaterialUniformData(const jshared_ptr<ShaderOld>& shader);
        void createMaterialUniformData_Matrix4(MaterialUniform* uniformValue, uint32 imageCount);
        void createMaterialUniformData_Image(MaterialUniform* uniformValue, uint32 imageCount);

        bool createDescriptorSets(const jshared_ptr<ShaderOld>& shader);

        void clearVulkanData() { clearVulkanData(getShader()); }
        void clearVulkanData(const jshared_ptr<ShaderOld>& shader);

        void updateMaterialUniformData_Matrix4(MaterialUniform* uniformValue, uint32 imageIndex);
        void updateMaterialUniformData_Image(int32 uniformIndex, MaterialUniform* uniformValue, uint32 imageIndex);
    };
}

#endif
