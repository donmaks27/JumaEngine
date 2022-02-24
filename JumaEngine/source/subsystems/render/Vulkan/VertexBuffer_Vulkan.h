// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/vertex/VertexBufferOld.h"
#include "VulkanContextObject.h"

#include "jutils/jshared_ptr.h"
#include "jutils/jstringID.h"

namespace JumaEngine
{
    struct RenderOptionsData_Vulkan;
    class VulkanCommandBuffer;
    class VulkanBuffer;

    class VertexBuffer_Vulkan final : public VertexBufferOld, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(VertexBuffer_Vulkan, VertexBufferOld)

    public:
        VertexBuffer_Vulkan() = default;
        virtual ~VertexBuffer_Vulkan() override;

        void render(const RenderOptionsData_Vulkan& data);

        const jstringID& getVertexName() const { return m_VertexName; }

    protected:

        virtual bool initInternal(const VertexBufferDataBase* data) override;
        virtual void clearInternal() override;

    private:

        jshared_ptr<VulkanBuffer> m_VertexBuffer = nullptr;
        jshared_ptr<VulkanBuffer> m_IndexBuffer = nullptr;

        jstringID m_VertexName = jstringID_NONE;


        void clearVulkanBuffers();
    };
}

#endif
