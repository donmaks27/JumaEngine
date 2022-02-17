// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include "VulkanRenderPassDescription.h"

namespace JumaEngine
{
    class VulkanRenderPass : public VulkanContextObject
    {
    public:
        VulkanRenderPass() = default;
        virtual ~VulkanRenderPass() override;

        bool init(const VulkanRenderPassDescription& description, render_pass_id_type typeID);

        VkRenderPass get() const { return m_RenderPass; }

        const VulkanRenderPassDescription& getDescription() const { return m_Description; }
        render_pass_id_type getTypeID() const { return m_TypeID; }

    protected:

        void clearInternal() override { clearRenderPass(); }

    private:

        VkRenderPass m_RenderPass = nullptr;

        VulkanRenderPassDescription m_Description;
        render_pass_id_type m_TypeID = INVALID_RENDER_PASS_TYPE_ID;


        void clearRenderPass();
    };
}

#endif
