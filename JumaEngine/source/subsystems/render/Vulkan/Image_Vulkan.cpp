// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Image_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "utils/jlog.h"
#include "RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    Image_Vulkan::~Image_Vulkan()
    {
        if (isValid())
        {
            clearImage();
        }
    }

    bool Image_Vulkan::init(const glm::uvec2& size, const uint32 mipLevels, const VkSampleCountFlagBits numSamples, const VkFormat format, 
        const jset<VulkanQueueType>& queues, const VkImageUsageFlags usage, const VmaMemoryUsage memoryUsage, const VkMemoryPropertyFlags properties)
    {
        jarray<uint32> uniqueQueueFamilies;
        for (auto& queue : queues)
        {
            uniqueQueueFamilies.addUnique(getRenderSubsystem()->getQueueFamilyIndex(queue));
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        if (uniqueQueueFamilies.getSize() > 1)
        {
            imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
            imageInfo.queueFamilyIndexCount = static_cast<uint32>(uniqueQueueFamilies.getSize());
            imageInfo.pQueueFamilyIndices = uniqueQueueFamilies.getData();
        }
        else
        {
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
        }
        imageInfo.samples = numSamples;
        imageInfo.flags = 0;

        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.flags = 0;
        allocationInfo.usage = memoryUsage;
        allocationInfo.preferredFlags = properties;
        return init(imageInfo, allocationInfo);
    }
    bool Image_Vulkan::init(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }

        const VkResult result = vmaCreateImage(getRenderSubsystem()->getAllocator(), &imageInfo, &allocationInfo, &m_Image, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create image. Code ") + TO_JSTR(result));
            return false;
        }

        m_MipLevels = imageInfo.mipLevels;
        m_ImageLayout = imageInfo.initialLayout;
        markAsInitialized({ imageInfo.extent.width, imageInfo.extent.height }, getImageFormatByVulkanFormat(imageInfo.format));
        return true;
    }

    bool Image_Vulkan::init(VkImage existingImage, const glm::uvec2& size, const VkFormat format, const uint32 mipLevels)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }
        const ImageFormat imageFormat = getImageFormatByVulkanFormat(format);
        if ((existingImage == nullptr) || (size.x == 0) || (size.y == 0) || (imageFormat == ImageFormat::None) || (mipLevels == 0))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }

        m_Image = existingImage;
        m_MipLevels = mipLevels;
        markAsInitialized(size, imageFormat);
        return true;
    }

    bool Image_Vulkan::initInternal(const glm::uvec2& size, ImageFormat format, const uint8* data)
    {
        return false;
    }

    bool Image_Vulkan::createImageView(const VkImageAspectFlags aspectFlags)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }
        if (m_ImageView != nullptr)
        {
            JUMA_LOG(warning, JSTR("Image view already created"));
            return false;
        }

        VkImageViewCreateInfo imageViewInfo{};
	    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	    imageViewInfo.image = m_Image;
	    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	    imageViewInfo.format = getVulkanFormatByImageFormat(getFormat());
	    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	    imageViewInfo.subresourceRange.aspectMask = aspectFlags;
	    imageViewInfo.subresourceRange.baseMipLevel = 0;
	    imageViewInfo.subresourceRange.levelCount = m_MipLevels;
	    imageViewInfo.subresourceRange.baseArrayLayer = 0;
	    imageViewInfo.subresourceRange.layerCount = 1;
        const VkResult result = vkCreateImageView(getRenderSubsystem()->getDevice(), &imageViewInfo, nullptr, &m_ImageView);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create image view. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }

    void Image_Vulkan::clearImage()
    {
        if (m_ImageView != nullptr)
        {
            vkDestroyImageView(getRenderSubsystem()->getDevice(), m_ImageView, nullptr);
            m_ImageView = nullptr;
        }
        if (m_Allocation != nullptr)
        {
            vmaDestroyImage(getRenderSubsystem()->getAllocator(), m_Image, m_Allocation);
            m_Allocation = nullptr;
        }
        m_Image = nullptr;
        m_MipLevels = 0;
        m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    ImageFormat Image_Vulkan::getImageFormatByVulkanFormat(const VkFormat format)
    {
        switch (format)
        {
        case VK_FORMAT_B8G8R8A8_SRGB: return ImageFormat::R8G8B8A8;
        case VK_FORMAT_D32_SFLOAT: return ImageFormat::SFLOAT32;
        case VK_FORMAT_D32_SFLOAT_S8_UINT: return ImageFormat::SFLOAT32UINT8;
        case VK_FORMAT_D24_UNORM_S8_UINT: return ImageFormat::UNORM24UINT8;
        default: ;
        }
        return ImageFormat::None;
    }
    VkFormat Image_Vulkan::getVulkanFormatByImageFormat(const ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::R8G8B8A8: return VK_FORMAT_B8G8R8A8_SRGB;
        case ImageFormat::SFLOAT32: return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::SFLOAT32UINT8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case ImageFormat::UNORM24UINT8: return VK_FORMAT_D24_UNORM_S8_UINT;
        default: ;
        }
        return VK_FORMAT_UNDEFINED;
    }
}

#endif