// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Texture_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "subsystems/render/texture/TextureData.h"
#include "vulkanObjects/VulkanImage.h"

namespace JumaEngine
{
    Texture_RenderAPIObject_Vulkan::~Texture_RenderAPIObject_Vulkan()
    {
        clearVulkanData();
    }

    void Texture_RenderAPIObject_Vulkan::clearVulkanData()
    {
        if (m_Image != nullptr)
        {
            delete m_Image;
            m_Image = nullptr;
        }
    }

    bool Texture_RenderAPIObject_Vulkan::initInternal()
    {
        const TextureData* data = getTextureData();
        const math::uvector2& size = data->getSize();
        const uint32 mipLevels = static_cast<uint32>(std::floor(std::log2(math::max(size.x, size.y)))) + 1;

        m_Image = getRenderSubsystemObject()->createVulkanObject<VulkanImage>();
        m_Image->init(
            data->getSize(), mipLevels, VK_SAMPLE_COUNT_1_BIT, GetVulkanFormatByTextureFormat(data->getFormat()), 
            { VulkanQueueType::Graphics, VulkanQueueType::Transfer }, 
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY, 0
        );
        if (!m_Image->isValid())
        {
            JUMA_LOG(error, JSTR("Failed to create vulkan image"));
            delete m_Image;
            return false;
        }

        if (!m_Image->copyDataToImage(data->getData()))
        {
            JUMA_LOG(error, JSTR("Failed to copy data to vulkan image"));
            delete m_Image;
            return false;
        }
        /*if (!m_Image->changeImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            JUMA_LOG(error, JSTR("Failed to change image layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL"));
            delete m_Image;
            return false;
        }*/
        if (!m_Image->generateMipmaps(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            JUMA_LOG(error, JSTR("Failed to generate mipmaps vulkan image"));
            delete m_Image;
            return false;
        }

        if (!m_Image->createImageView(VK_IMAGE_ASPECT_COLOR_BIT) || !m_Image->createSampler())
        {
            JUMA_LOG(error, JSTR("Failed to create image view or sampler for vulkan image"));
            delete m_Image;
            return false;
        }
        return true;
    }
}

#endif
