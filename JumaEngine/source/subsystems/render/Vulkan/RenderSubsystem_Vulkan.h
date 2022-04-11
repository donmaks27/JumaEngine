// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderSubsystem.h"

#include "vma_lib.h"
#include <vulkan/vulkan_core.h>

#include "VulkanContextObject.h"
#include "jutils/jarray.h"
#include "jutils/jmap.h"
#include "jutils/jstringID.h"
#include "vulkanObjects/VulkanQueueType.h"
#include "vulkanObjects/VulkanRenderPassDescription.h"

namespace JumaEngine
{
    class VulkanRenderPass;
    class VertexBufferDataBase;
    class VulkanCommandPool;
    class VulkanQueue;

    struct VertexDescription_Vulkan
    {
        VkVertexInputBindingDescription binding = VkVertexInputBindingDescription();
        jarray<VkVertexInputAttributeDescription> attributes;
    };

    class RenderSubsystem_RenderAPIObject_Vulkan final : public RenderSubsystem_RenderAPIObject
    {
        using Super = RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem_RenderAPIObject_Vulkan() = default;
        virtual ~RenderSubsystem_RenderAPIObject_Vulkan() override;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<VulkanContextObjectBase, T>)>
        T* createVulkanObject() { return registerVulkanObject(new T()); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<VulkanContextObjectBase, T>)>
        T* registerVulkanObject(T* vulkanContextObject)
        {
            if (vulkanContextObject != nullptr)
            {
                vulkanContextObject->m_RenderSubsystem = this;
            }
            return vulkanContextObject;
        }

        VkInstance getVulkanInstance() const { return m_VulkanInstance; }
        VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        VkDevice getDevice() const { return m_Device; }
        VmaAllocator getAllocator() const { return m_Allocator; }

        bool pickDepthFormat(VkFormat& outFormat) const;

        uint32 getQueueFamilyIndex(const VulkanQueueType queueType) const { return m_QueueFamilyIndices[queueType]; }
        VulkanQueue* getQueue(const VulkanQueueType queueType) const { return m_Queues[m_QueueFamilyIndices[queueType]]; }
        VulkanCommandPool* getCommandPool(const VulkanQueueType queueType) const { return m_CommandPools[queueType]; }

        constexpr static int8 getMaxRenderFrameCount() { return 2; }
        int8 getRenderFrameCount() const { return m_RenderFrameCount; }
        int8 getRenderFrameIndex() const { return m_RenderFrameIndex; }
        int8 updateRenderFrameIndex() { return m_RenderFrameIndex = static_cast<int8>((m_RenderFrameIndex + 1) % m_RenderFrameCount); }

        void registerVertexType(const VertexBufferDataBase* vertexBufferData);
        const VertexDescription_Vulkan* findVertexDescription(const jstringID& vertexName) const { return m_RegisteredVertexTypes.find(vertexName); }
        
        VulkanRenderPass* getRenderPass(const VulkanRenderPassDescription& description);

    protected:

        virtual bool initInternal() override;

        virtual Shader_RenderAPIObject* createShaderObject() override;
        virtual Material_RenderAPIObject* createMaterialObject() override;
        virtual VertexBuffer_RenderAPIObject* createVertexBufferObject() override;
        virtual Texture_RenderAPIObject* createTextureObject() override;
        virtual RenderTarget_RenderAPIObject* createRenderTargetObject() override;
        virtual RenderPipeline_RenderAPIObject* createRenderPipelineObject() override;

        virtual void waitForRenderFinish() override;

    private:

        const jarray<const char*> m_VulkanDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkInstance m_VulkanInstance = nullptr;
        VmaAllocator m_Allocator = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkDevice m_Device = nullptr;

#if JDEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
#endif

        jmap<VulkanQueueType, uint32> m_QueueFamilyIndices;
        jmap<uint32, VulkanQueue*> m_Queues;
        jmap<VulkanQueueType, VulkanCommandPool*> m_CommandPools;

        int8 m_RenderFrameCount = 0;
        int8 m_RenderFrameIndex = 0;

        jmap<jstringID, VertexDescription_Vulkan> m_RegisteredVertexTypes;

        juid<render_pass_id_type> m_RenderPassTypeIDs;
        jmap<VulkanRenderPassDescription, render_pass_id_type, VulkanRenderPassDescription::CompatiblePredicate> m_RenderPassTypes;
        jmap<VulkanRenderPassDescription, VulkanRenderPass*, VulkanRenderPassDescription::EqualPredicate> m_RenderPasses;


        bool initVulkan();
        void clearVulkan();

        bool createVulkanInstance();
        jarray<const char*> getRequiredVulkanExtensions() const;

        bool pickPhysicalDevice();
        static bool getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, jmap<VulkanQueueType, uint32>& outQueueIndices);

        bool createDevice();
        bool createCommandPools();
    };
}

#endif
