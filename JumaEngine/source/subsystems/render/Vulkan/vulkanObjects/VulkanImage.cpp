// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanImage.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"

namespace JumaEngine
{
    VulkanImage::~VulkanImage()
    {
        clearVulkanData();
    }

    void VulkanImage::clearVulkanData()
    {
        VkDevice device = getRenderSubsystem()->getDevice();

        if (m_Sampler != nullptr)
        {
            vkDestroySampler(device, m_Sampler, nullptr);
            m_Sampler = nullptr;
        }

        if (m_ImageView != nullptr)
        {
            vkDestroyImageView(device, m_ImageView, nullptr);
            m_ImageView = nullptr;
        }

        if (m_Allocation != nullptr)
        {
            vmaDestroyImage(getRenderSubsystem()->getAllocator(), m_Image, m_Allocation);
            m_Allocation = nullptr;
        }
        m_Image = nullptr;

        m_Size = { 0, 0 };
        m_Format = VkFormat::VK_FORMAT_UNDEFINED;
        m_MipLevels = 0;
        m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    bool VulkanImage::init(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }
        return initInternal(imageInfo, allocationInfo);
    }
    bool VulkanImage::init(const math::uvector2& size, const uint32 mipLevels, 
        const VkSampleCountFlagBits numSamples, const VkFormat format, 
        std::initializer_list<VulkanQueueType> queues, const VkImageUsageFlags usage,
        const VmaMemoryUsage memoryUsage, const VkMemoryPropertyFlags properties)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }

        jarray<uint32> uniqueQueueFamilies;
        uniqueQueueFamilies.reserve(static_cast<int32>(queues.size()));
        for (const auto& queueType : queues)
        {
            uniqueQueueFamilies.addUnique(getRenderSubsystem()->getQueueFamilyIndex(queueType));
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
        return initInternal(imageInfo, allocationInfo);
    }
    bool VulkanImage::initInternal(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo)
    {
        const VkResult result = vmaCreateImage(getRenderSubsystem()->getAllocator(), &imageInfo, &allocationInfo, &m_Image, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create image"), result);
            return false;
        }

        m_MipLevels = imageInfo.mipLevels;
        m_Size = { imageInfo.extent.width, imageInfo.extent.height };
        m_Format = imageInfo.format;
        m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        markAsInitialized();
        return true;
    }

    bool VulkanImage::init(VkImage existingImage, const math::uvector2& size, const VkFormat format, 
        const uint32 mipLevels)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }
        if ((existingImage == nullptr) || (size.x == 0) || (size.y == 0) || (mipLevels == 0))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }

        m_Image = existingImage;
        m_Size = size;
        m_Format = format;
        m_MipLevels = mipLevels;
        m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        markAsInitialized();
        return true;
    }

    bool VulkanImage::createImageView(const VkImageAspectFlags aspectFlags)
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
	    imageViewInfo.format = m_Format;
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
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create image view"), result);
            return false;
        }
        return true;
    }

    bool VulkanImage::createSampler()
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
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
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
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create image sampler"), result);
            return false;
        }
        return true;
    }

    jshared_ptr<VulkanCommandBuffer> VulkanImage::createCommandBuffer(const VulkanQueueType queueType) const
    {
        const jshared_ptr<VulkanCommandPool>& commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Graphics);
        jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if (commandBuffer == nullptr)
        {
            return nullptr;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);
        return commandBuffer;
    }
    void VulkanImage::submitCommandBuffer(const jshared_ptr<VulkanCommandBuffer>& commandBuffer) const
    {
        vkEndCommandBuffer(commandBuffer->get());
        commandBuffer->submit(true);
    }

    bool VulkanImage::changeImageLayout(VkCommandBuffer commandBuffer, const VkImageLayout newLayout, 
        const VkAccessFlags srcAccess, const VkPipelineStageFlags srcStage, 
        const VkAccessFlags dstAccess, const VkPipelineStageFlags dstStage,
        const uint32 mipLevel, const uint32 levelCount)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(warning, JSTR("Invalid command buffer"));
            return false;
        }

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
        imageBarrier.srcAccessMask = srcAccess;
        imageBarrier.dstAccessMask = dstAccess;
        vkCmdPipelineBarrier(
            commandBuffer,
            srcStage, dstStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageBarrier
        );

        m_ImageLayout = newLayout;
        return true;
    }
    bool VulkanImage::changeImageLayout(VkCommandBuffer commandBuffer, const VkImageLayout newLayout)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }

        if ((m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
        {
            return changeImageLayout(commandBuffer, newLayout, 
                0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
            );
        }
        if ((m_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            return changeImageLayout(commandBuffer, newLayout, 
                VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
        }
        return false;
    }

    bool VulkanImage::changeImageLayout(const VkImageLayout newLayout, 
        const VkAccessFlags srcAccess, const VkPipelineStageFlags srcStage,
        const VkAccessFlags dstAccess, const VkPipelineStageFlags dstStage, 
        const uint32 mipLevel, const uint32 levelCount)
    {
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createCommandBuffer(VulkanQueueType::Graphics);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        if (!changeImageLayout(commandBuffer->get(), newLayout, srcAccess, srcStage, dstAccess, dstStage, mipLevel, levelCount))
        {
            return false;
        }
        submitCommandBuffer(commandBuffer);
        return true;
    }
    bool VulkanImage::changeImageLayout(const VkImageLayout newLayout, 
        const VkAccessFlags srcAccess, const VkPipelineStageFlags srcStage,
        const VkAccessFlags dstAccess, const VkPipelineStageFlags dstStage)
    {
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createCommandBuffer(VulkanQueueType::Graphics);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        if (!changeImageLayout(commandBuffer->get(), newLayout, srcAccess, srcStage, dstAccess, dstStage))
        {
            return false;
        }
        submitCommandBuffer(commandBuffer);
        return true;
    }
    bool VulkanImage::changeImageLayout(const VkImageLayout newLayout)
    {
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createCommandBuffer(VulkanQueueType::Graphics);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        if (!changeImageLayout(commandBuffer->get(), newLayout))
        {
            return false;
        }
        submitCommandBuffer(commandBuffer);
        return true;
    }

    bool VulkanImage::generateMipmaps(VkCommandBuffer commandBuffer, const VkImageLayout finalLayout)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(warning, JSTR("Invalid command buffer"));
            return false;
        }

        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(getRenderSubsystem()->getPhysicalDevice(), m_Format, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            return false;
        }

        math::ivector2 mipmapSize = m_Size;

        VkImageMemoryBarrier imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.image = m_Image;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;
        imageBarrier.subresourceRange.levelCount = 1;
        for (uint32 mipLevel = 1; mipLevel < m_MipLevels; mipLevel++)
        {
            imageBarrier.subresourceRange.baseMipLevel = mipLevel - 1;
            imageBarrier.oldLayout = m_ImageLayout;
            imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &imageBarrier
            );

            VkImageBlit imageBlit{};
            imageBlit.srcOffsets[0] = { 0, 0, 0 };
            imageBlit.srcOffsets[1] = { mipmapSize.x, mipmapSize.y, 1 };
            imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.srcSubresource.mipLevel = mipLevel - 1;
            imageBlit.srcSubresource.baseArrayLayer = 0;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.dstOffsets[0] = { 0, 0, 0 };
            imageBlit.dstOffsets[1] = { mipmapSize.x > 1 ? mipmapSize.x / 2 : 1, mipmapSize.y > 1 ? mipmapSize.y / 2 : 1, 1 };
            imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.dstSubresource.mipLevel = mipLevel;
            imageBlit.dstSubresource.baseArrayLayer = 0;
            imageBlit.dstSubresource.layerCount = 1;
            vkCmdBlitImage(commandBuffer,
                m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &imageBlit, VK_FILTER_LINEAR
            );

            imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            imageBarrier.newLayout = finalLayout;
            imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer,
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
        imageBarrier.newLayout = finalLayout;
        imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &imageBarrier
        );

        m_ImageLayout = finalLayout;
        return true;
    }
    bool VulkanImage::generateMipmaps(const VkImageLayout finalLayout)
    {
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createCommandBuffer(VulkanQueueType::Graphics);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        if (!generateMipmaps(commandBuffer->get(), finalLayout))
        {
            return false;
        }
        submitCommandBuffer(commandBuffer);
        return true;
    }

    bool VulkanImage::copyDataToImage(const uint8* data)
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Image not initialized"));
            return false;
        }

        VulkanBuffer* stagingBuffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
        stagingBuffer->init(
            static_cast<uint32>(m_Size.x) * m_Size.y * GetTextureFormatSize(GetTextureFormatByVulkanFormat(m_Format)), 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, {}, 
            VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        if (!stagingBuffer->isValid() || !stagingBuffer->setData(data))
        {
            delete stagingBuffer;
            return false;
        }

        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createCommandBuffer(VulkanQueueType::Transfer);
        if (commandBuffer == nullptr)
        {
            delete stagingBuffer;
            return false;
        }

        if (!changeImageLayout(commandBuffer->get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
        {
            delete stagingBuffer;
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
        imageCopy.imageExtent = { m_Size.x, m_Size.y, 1 };
        vkCmdCopyBufferToImage(commandBuffer->get(), 
            stagingBuffer->get(), m_Image, 
            m_ImageLayout, 1, &imageCopy
        );

        submitCommandBuffer(commandBuffer);
        delete stagingBuffer;
        return true;
    }
}

#endif
