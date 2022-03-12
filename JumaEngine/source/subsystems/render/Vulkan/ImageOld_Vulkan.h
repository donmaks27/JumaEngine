// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/ImageOld.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include "vma_lib.h"

#include "vulkanObjects/VulkanQueueType.h"
#include "jutils/jset.h"
#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    class VulkanBuffer;

    class ImageOld_Vulkan final : public ImageOld, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(ImageOld_Vulkan, ImageOld)

    public:
        ImageOld_Vulkan() = default;
        virtual ~ImageOld_Vulkan() override;

        bool init(const math::uvector2& size, uint32 mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, const jset<VulkanQueueType>& queues, 
            VkImageUsageFlags usage, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags properties);
        bool init(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo);
        
        bool init(VkImage existingImage, const math::uvector2& size, TextureFormat format, uint32 mipLevels = 1);

        bool createImageView(VkImageAspectFlags aspectFlags);
        
        static TextureFormat getImageFormatByVulkanFormat(VkFormat format);
        static VkFormat getVulkanFormatByImageFormat(TextureFormat format);

        VkImage get() const { return m_Image; }
        VkImageView getImageView() const { return m_ImageView; }
        VkSampler getSampler() const { return m_Sampler; }

        uint32 getMipLevels() const { return m_MipLevels; }

    protected:

        virtual bool initInternal(const math::uvector2& size, TextureFormat format, const uint8* data) override;

        virtual void clearInternal() override { clearImage(); }

    private:

        VkImage m_Image = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VkImageView m_ImageView = nullptr;
        VkSampler m_Sampler = nullptr;

        uint32 m_MipLevels = 0;
        VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;


        bool copyDataToImage(const math::uvector2& size, TextureFormat format, const uint8* data);
        bool changeImageLayout(VkImageLayout newLayout);
        bool generateMipmaps(const math::uvector2& size, TextureFormat format, VkImageLayout newLayout);

        bool createImageView(TextureFormat format, VkImageAspectFlags aspectFlags);
        bool createSampler();

        void clearImage();
    };
}

#endif
