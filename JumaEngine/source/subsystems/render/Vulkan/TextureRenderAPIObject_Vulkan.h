// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Texture.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VulkanImage;

    class TextureRenderAPIObject_Vulkan : public TextureRenderAPIObject, public VulkanContextObjectBase
    {
    public:
        TextureRenderAPIObject_Vulkan() = default;
        virtual ~TextureRenderAPIObject_Vulkan() override;

        VulkanImage* getImage() const { return m_Image; }

    protected:

        virtual bool initInternal() override;

    private:

        VulkanImage* m_Image = nullptr;


        void clearVulkanData();
    };
}

#endif
