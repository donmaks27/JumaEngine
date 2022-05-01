﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include "subsystems/render/Vulkan/vma_lib.h"

#include "jutils/math/vector2.h"
#include "subsystems/render/texture/TextureFormat.h"
#include "subsystems/render/texture/TextureSamples.h"
#include "VulkanQueueType.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;

    constexpr TextureFormat GetTextureFormatByVulkanFormat(VkFormat format);
    constexpr VkFormat GetVulkanFormatByTextureFormat(TextureFormat format);

    constexpr VkSampleCountFlagBits GetSampleCountByTextureSamples(TextureSamples samples);

    class VulkanImage : public VulkanContextObject
    {
    public:
        VulkanImage() = default;
        virtual ~VulkanImage() override;

        bool init(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo);
        bool init(const math::uvector2& size, uint32 mipLevels, VkSampleCountFlagBits numSamples, 
            VkFormat format, std::initializer_list<VulkanQueueType> queues, VkImageUsageFlags usage, 
            VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags properties);
        bool init(VkImage existingImage, const math::uvector2& size, VkFormat format, uint32 mipLevels = 1);

        bool createImageView(VkImageAspectFlags aspectFlags);

        bool createSampler();

        const math::uvector2& getSize() const { return m_Size; }

        VkImage get() const { return m_Image; }
        VkImageView getImageView() const { return m_ImageView; }
        VkSampler getSampler() const { return m_Sampler; }

        bool changeImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, 
            VkAccessFlags srcAccess, VkPipelineStageFlags srcStage, 
            VkAccessFlags dstAccess, VkPipelineStageFlags dstStage,
            uint32 mipLevel, uint32 levelCount);
        bool changeImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout, 
            VkAccessFlags srcAccess, VkPipelineStageFlags srcStage, 
            VkAccessFlags dstAccess, VkPipelineStageFlags dstStage)
        {
            return changeImageLayout(commandBuffer, newLayout, 
                srcAccess, srcStage, dstAccess, dstStage,
                0, m_MipLevels
            );
        }
        bool changeImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout);

        bool changeImageLayout(VkImageLayout newLayout, 
            VkAccessFlags srcAccess, VkPipelineStageFlags srcStage, 
            VkAccessFlags dstAccess, VkPipelineStageFlags dstStage,
            uint32 mipLevel, uint32 levelCount);
        bool changeImageLayout(VkImageLayout newLayout, 
            VkAccessFlags srcAccess, VkPipelineStageFlags srcStage, 
            VkAccessFlags dstAccess, VkPipelineStageFlags dstStage);
        bool changeImageLayout(VkImageLayout newLayout);

        bool generateMipmaps(VkCommandBuffer commandBuffer, VkImageLayout finalLayout);
        bool generateMipmaps(VkCommandBuffer commandBuffer) { return generateMipmaps(commandBuffer, m_ImageLayout); }
        bool generateMipmaps(VkImageLayout finalLayout);
        bool generateMipmaps() { return generateMipmaps(m_ImageLayout); }

        bool copyDataToImage(const uint8* data);

    protected:

        virtual void clearInternal() override { clearVulkanData(); }

    private:

        VkImage m_Image = nullptr;
        VmaAllocation m_Allocation = nullptr;
        VkImageView m_ImageView = nullptr;
        VkSampler m_Sampler = nullptr;

        math::uvector2 m_Size = { 0, 0 };
        VkFormat m_Format = VkFormat::VK_FORMAT_UNDEFINED;
        uint32 m_MipLevels = 0;
        VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;


        void clearVulkanData();

        bool initInternal(const VkImageCreateInfo& imageInfo, const VmaAllocationCreateInfo& allocationInfo);

        VulkanCommandBuffer* createCommandBuffer(VulkanQueueType queueType) const;
        void submitCommandBuffer(VulkanCommandBuffer* commandBuffer) const;
    };

    constexpr TextureFormat GetTextureFormatByVulkanFormat(const VkFormat format)
    {
        switch (format)
        {
        case VK_FORMAT_R8G8B8A8_SRGB: return TextureFormat::RGBA_UINT8;
        case VK_FORMAT_B8G8R8A8_SRGB: return TextureFormat::BGRA_UINT8;
        case VK_FORMAT_D32_SFLOAT: return TextureFormat::DEPTH_FLOAT32;
        case VK_FORMAT_D32_SFLOAT_S8_UINT: return TextureFormat::DEPTH_FLOAT32_STENCIL_UINT8;
        case VK_FORMAT_D24_UNORM_S8_UINT: return TextureFormat::DEPTH_UNORM24_STENCIL_UINT8;
        default: ;
        }
        return TextureFormat::None;
    }
    constexpr VkFormat GetVulkanFormatByTextureFormat(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA_UINT8: return VK_FORMAT_R8G8B8A8_SRGB;
        case TextureFormat::BGRA_UINT8: return VK_FORMAT_B8G8R8A8_SRGB;
        case TextureFormat::DEPTH_FLOAT32: return VK_FORMAT_D32_SFLOAT;
        case TextureFormat::DEPTH_FLOAT32_STENCIL_UINT8: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case TextureFormat::DEPTH_UNORM24_STENCIL_UINT8: return VK_FORMAT_D24_UNORM_S8_UINT;
        default: ;
        }
        return VK_FORMAT_UNDEFINED;
    }

    constexpr VkSampleCountFlagBits GetSampleCountByTextureSamples(const TextureSamples samples)
    {
        switch (samples)
        {
        case TextureSamples::X2:  return VK_SAMPLE_COUNT_2_BIT;
        case TextureSamples::X4:  return VK_SAMPLE_COUNT_4_BIT;
        case TextureSamples::X8:  return VK_SAMPLE_COUNT_8_BIT;
        case TextureSamples::X16: return VK_SAMPLE_COUNT_16_BIT;
        case TextureSamples::X32: return VK_SAMPLE_COUNT_32_BIT;
        case TextureSamples::X64: return VK_SAMPLE_COUNT_64_BIT;
        default: ;
        }
        return VK_SAMPLE_COUNT_1_BIT;
    }
}

#endif
