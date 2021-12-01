// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Image.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include "vma_lib.h"

#include "vulkanObjects/VulkanQueueType.h"
#include "utils/jset.h"
#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class VulkanBuffer;

    class Image_Vulkan final : public Image, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(Image_Vulkan, Image)

    public:
        Image_Vulkan() = default;
        virtual ~Image_Vulkan() override;

        bool init(const math::uvector2& size, uint32 mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, const jset<VulkanQueueType>& queues, 
            VkImageUsageFlags usage, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags properties);
        bool init(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo);
        
        bool init(VkImage existingImage, const math::uvector2& size, ImageFormat format, uint32 mipLevels = 1);

        bool createImageView(VkImageAspectFlags aspectFlags);
        
        static ImageFormat getImageFormatByVulkanFormat(VkFormat format);
        static VkFormat getVulkanFormatByImageFormat(ImageFormat format);

        VkImage get() const { return m_Image; }
        VkImageView getImageView() const { return m_ImageView; }
        VkSampler getSampler() const { return m_Sampler; }

        uint32 getMipLevels() const { return m_MipLevels; }

    protected:

        virtual bool initInternal(const math::uvector2& size, ImageFormat format, const uint8* data) override;

        virtual void clearInternal() override { clearImage(); }

    private:

        VkImage m_Image = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VkImageView m_ImageView = nullptr;
        VkSampler m_Sampler = nullptr;

        uint32 m_MipLevels = 0;
        VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;


        bool changeImageLayout(VkImageLayout newLayout);
        bool copyDataToImage(const math::uvector2& size, ImageFormat format, const uint8* data);
        bool generateMipmaps(const math::uvector2& size, ImageFormat format, VkImageLayout newLayout);

        bool createImageView(ImageFormat format, VkImageAspectFlags aspectFlags);
        bool createSampler();

        void clearImage();
    };
}

#endif
