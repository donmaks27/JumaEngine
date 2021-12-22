// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Image_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "utils/jlog.h"
#include "RenderSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanCommandPool.h"

namespace JumaEngine
{
    Image_Vulkan::~Image_Vulkan()
    {
        if (isValid())
        {
            clearImage();
        }
    }

    bool Image_Vulkan::init(const math::uvector2& size, const uint32 mipLevels, const VkSampleCountFlagBits numSamples, const VkFormat format, 
        const jset<VulkanQueueType>& queues, const VkImageUsageFlags usage, const VmaMemoryUsage memoryUsage, const VkMemoryPropertyFlags properties)
    {
        jarray<uint32> uniqueQueueFamilies;
        uniqueQueueFamilies.reserve(queues.getSize());
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
            JUMA_VULKAN_ERROR_LOG(JSTR("create image"), result);
            return false;
        }

        m_MipLevels = imageInfo.mipLevels;
        m_ImageLayout = imageInfo.initialLayout;
        m_Size = { imageInfo.extent.width, imageInfo.extent.height };
        m_Format = getImageFormatByVulkanFormat(imageInfo.format);
        markAsInitialized();
        return true;
    }

    bool Image_Vulkan::init(VkImage existingImage, const math::uvector2& size, const ImageFormat format, const uint32 mipLevels)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }
        if ((existingImage == nullptr) || (size.x == 0) || (size.y == 0) || (format == ImageFormat::None) || (mipLevels == 0))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }

        m_Image = existingImage;
        m_MipLevels = mipLevels;
        m_Size = size;
        m_Format = format;
        markAsInitialized();
        return true;
    }

    bool Image_Vulkan::initInternal(const math::uvector2& size, const ImageFormat format, const uint8* data)
    {
        m_MipLevels = static_cast<uint32>(std::floor(std::log2(math::max(size.x, size.y)))) + 1;
        m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        const uint32 queueFamilyIndices[2] = {
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Graphics),
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Transfer)
        };

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = m_MipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = getVulkanFormatByImageFormat(format);
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = m_ImageLayout;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        if (queueFamilyIndices[0] != queueFamilyIndices[1])
        {
            imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
            imageInfo.queueFamilyIndexCount = 2;
            imageInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.queueFamilyIndexCount = 0;
            imageInfo.pQueueFamilyIndices = nullptr;
        }
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.flags = 0;
        allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocationInfo.preferredFlags = 0;
        const VkResult result = vmaCreateImage(getRenderSubsystem()->getAllocator(), &imageInfo, &allocationInfo, &m_Image, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("create image"), result);
            return false;
        }

        if (data != nullptr)
        {
            if (!copyDataToImage(size, format, data) || !generateMipmaps(size, format, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
            {
                return false;
            }
        }
        return createImageView(format, VK_IMAGE_ASPECT_COLOR_BIT) && createSampler();
    }
    bool Image_Vulkan::copyDataToImage(const math::uvector2& size, const ImageFormat format, const uint8* data)
    {
        if (!changeImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
        {
            return false;
        }

        const jshared_ptr<VulkanBuffer> stagingBuffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
        stagingBuffer->init(
            size.x * size.y * getPixelSize(format), 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, {}, 
            VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        if (!stagingBuffer->isValid() || !stagingBuffer->setData(data))
        {
            return false;
        }

        const jshared_ptr<VulkanCommandPool>& commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Transfer);
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if (commandBuffer == nullptr)
        {
            return false;
        }

        VkBufferImageCopy imageCopy{};
        imageCopy.bufferOffset = 0;
        imageCopy.bufferRowLength = 0;
        imageCopy.bufferImageHeight = 0;
        imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopy.imageSubresource.mipLevel = 0;
        imageCopy.imageSubresource.baseArrayLayer = 0;
        imageCopy.imageSubresource.layerCount = 1;
        imageCopy.imageOffset = { 0, 0, 0 };
        imageCopy.imageExtent = { size.x, size.y, 1 };

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);
        vkCmdCopyBufferToImage(commandBuffer->get(), stagingBuffer->get(), m_Image, m_ImageLayout, 1, &imageCopy);
        vkEndCommandBuffer(commandBuffer->get());
        commandBuffer->submit(true);
        return true;
    }
    bool Image_Vulkan::changeImageLayout(const VkImageLayout newLayout)
    {
        if (m_ImageLayout == newLayout)
        {
            return true;
        }

        VkPipelineStageFlags sourceStage, destinationStage;
        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.oldLayout = m_ImageLayout;
        imageBarrier.newLayout = newLayout;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = m_Image;
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = m_MipLevels;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;
        if ((m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
        {
            imageBarrier.srcAccessMask = 0;
            imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if ((m_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            return false;
        }

        const jshared_ptr<VulkanCommandPool>& commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Graphics);
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if (commandBuffer == nullptr)
        {
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);
        vkCmdPipelineBarrier(
            commandBuffer->get(),
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageBarrier
        );
        vkEndCommandBuffer(commandBuffer->get());
        commandBuffer->submit(true);

        m_ImageLayout = newLayout;
        return true;
    }
    bool Image_Vulkan::generateMipmaps(const math::uvector2& size, const ImageFormat format, const VkImageLayout newLayout)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(getRenderSubsystem()->getPhysicalDevice(), getVulkanFormatByImageFormat(format), &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            return false;
        }

        const jshared_ptr<VulkanCommandPool>& commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Graphics);
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if (commandBuffer == nullptr)
        {
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);

        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.image = m_Image;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;
        imageBarrier.subresourceRange.levelCount = 1;

        math::uvector2 mipmapSize = size;
        for (uint32 mipLevel = 1; mipLevel < m_MipLevels; mipLevel++)
        {
            imageBarrier.subresourceRange.baseMipLevel = mipLevel - 1;
            imageBarrier.oldLayout = m_ImageLayout;
            imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer->get(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &imageBarrier
            );

            VkImageBlit imageBlit{};
            imageBlit.srcOffsets[0] = { 0, 0, 0 };
            imageBlit.srcOffsets[1] = { static_cast<int32>(mipmapSize.x), static_cast<int32>(mipmapSize.y), 1 };
            imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.srcSubresource.mipLevel = mipLevel - 1;
            imageBlit.srcSubresource.baseArrayLayer = 0;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.dstOffsets[0] = { 0, 0, 0 };
            imageBlit.dstOffsets[1] = { static_cast<int32_t>(mipmapSize.x > 1 ? mipmapSize.x / 2 : 1), static_cast<int32_t>(mipmapSize.y > 1 ? mipmapSize.y / 2 : 1), 1 };
            imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.dstSubresource.mipLevel = mipLevel;
            imageBlit.dstSubresource.baseArrayLayer = 0;
            imageBlit.dstSubresource.layerCount = 1;
            vkCmdBlitImage(commandBuffer->get(),
                m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &imageBlit, VK_FILTER_LINEAR
            );

            imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageBarrier.newLayout = newLayout;
            imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer->get(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &imageBarrier
            );

            if (mipmapSize.x > 1)
            {
                mipmapSize.x /= 2;
            }
            if (mipmapSize.y > 1)
            {
                mipmapSize.y /= 2;
            }
        }

        imageBarrier.subresourceRange.baseMipLevel = m_MipLevels - 1;
        imageBarrier.oldLayout = m_ImageLayout;
        imageBarrier.newLayout = newLayout;
        imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer->get(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &imageBarrier
        );

        vkEndCommandBuffer(commandBuffer->get());
        commandBuffer->submit(true);

        m_ImageLayout = newLayout;
        return true;
    }

    bool Image_Vulkan::createImageView(const VkImageAspectFlags aspectFlags)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }
        return createImageView(getFormat(), aspectFlags);
    }
    bool Image_Vulkan::createImageView(const ImageFormat format, const VkImageAspectFlags aspectFlags)
    {
        if (m_ImageView != nullptr)
        {
            JUMA_LOG(warning, JSTR("Image view already created"));
            return false;
        }

        VkImageViewCreateInfo imageViewInfo{};
	    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	    imageViewInfo.image = m_Image;
	    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	    imageViewInfo.format = getVulkanFormatByImageFormat(format);
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
            JUMA_VULKAN_ERROR_LOG(JSTR("create image view"), result);
            return false;
        }
        return true;
    }
    bool Image_Vulkan::createSampler()
    {
        if (m_Sampler != nullptr)
        {
            JUMA_LOG(warning, JSTR("Image sampler already created"));
            return false;
        }

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(getRenderSubsystem()->getPhysicalDevice(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(m_MipLevels);
        samplerInfo.mipLodBias = 0.0f;
        const VkResult result = vkCreateSampler(getRenderSubsystem()->getDevice(), &samplerInfo, nullptr, &m_Sampler);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("create image sampler"), result);
            return false;
        }
        return true;
    }

    void Image_Vulkan::clearImage()
    {
        if (m_Sampler != nullptr)
        {
            vkDestroySampler(getRenderSubsystem()->getDevice(), m_Sampler, nullptr);
        }
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
        case VK_FORMAT_R8G8B8A8_SRGB: return ImageFormat::RGBA;
        case VK_FORMAT_B8G8R8A8_SRGB: return ImageFormat::BGRA;
        case VK_FORMAT_D32_SFLOAT: return ImageFormat::SFLOAT32;
        case VK_FORMAT_D32_SFLOAT_S8_UINT: return ImageFormat::SFLOAT32_UINT8;
        case VK_FORMAT_D24_UNORM_S8_UINT: return ImageFormat::UNORM24_UINT8;
        default: ;
        }
        return ImageFormat::None;
    }
    VkFormat Image_Vulkan::getVulkanFormatByImageFormat(const ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::RGBA: return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::BGRA: return VK_FORMAT_B8G8R8A8_SRGB;
        case ImageFormat::SFLOAT32: return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::SFLOAT32_UINT8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case ImageFormat::UNORM24_UINT8: return VK_FORMAT_D24_UNORM_S8_UINT;
        default: ;
        }
        return VK_FORMAT_UNDEFINED;
    }
}

#endif