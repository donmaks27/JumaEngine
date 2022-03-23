// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderSubsystem.h"

#include "vma_lib.h"
#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/jmap.h"
#include "jutils/jstringID.h"
#include "jutils/jshared_ptr.h"
#include "vulkanObjects/VulkanQueueType.h"
#include "VulkanContextObject.h"
#include "vulkanObjects/VulkanRenderPassDescription.h"

namespace JumaEngine
{
    class VertexBufferDataBase;
    struct VertexComponentDescription;
    class VulkanRenderPass;
    class VulkanSwapchain;
    class VulkanCommandPool;
    class VulkanQueue;

    struct VertexDescription_Vulkan
    {
        VertexDescription_Vulkan() = default;
        VertexDescription_Vulkan(uint32 vertexSize, const jarray<VertexComponentDescription>& vertexComponents);
        VertexDescription_Vulkan(const VertexDescription_Vulkan&) = default;
        VertexDescription_Vulkan(VertexDescription_Vulkan&&) noexcept = default;

        VertexDescription_Vulkan& operator=(const VertexDescription_Vulkan&) = default;
        VertexDescription_Vulkan& operator=(VertexDescription_Vulkan&&) noexcept = default;

        VkVertexInputBindingDescription binding = VkVertexInputBindingDescription();
        jarray<VkVertexInputAttributeDescription> attributes;
    };

    class RenderSubsystem_Vulkan : public RenderSubsystem
    {
        JUMAENGINE_CLASS(RenderSubsystem_Vulkan, RenderSubsystem)

    public:
        RenderSubsystem_Vulkan() = default;
        virtual ~RenderSubsystem_Vulkan() override = default;

        VkInstance getVulkanInstance() const { return m_VulkanInstance; }
        VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        VkDevice getDevice() const { return m_Device; }
        VmaAllocator getAllocator() const { return m_Allocator; }

        bool pickDepthFormat(VkFormat& outFormat) const;

        uint32 getQueueFamilyIndex(const VulkanQueueType queueType) const { return m_QueueFamilyIndices[queueType]; }
        const jshared_ptr<VulkanQueue>& getQueue(const VulkanQueueType queueType) const { return m_Queues[m_QueueFamilyIndices[queueType]]; }
        VulkanCommandPool* getCommandPool(const VulkanQueueType queueType) const { return m_CommandPools[queueType]; }
        VulkanSwapchain* getSwapchain() const;

        virtual void render() override;

        virtual void onEnginePreTerminate() override;

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

        virtual ShaderObject* createShaderObject() override;
        virtual MaterialObject* createMaterialObject() override;
        virtual VertexBufferObject* createVertexBufferObject() override;
        virtual TextureObject* createTextureObject() override;

        void registerVertexType(const VertexBufferDataBase* vertexBufferData);
        const VertexDescription_Vulkan* findVertexDescription(const jstringID& vertexName) const { return m_RegisteredVertexTypes.find(vertexName); }

        VulkanRenderPass* createRenderPass(const VulkanRenderPassDescription& description);

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

    private:

        struct RenderPassTypeContainer
        {
            render_pass_id_type id = INVALID_RENDER_PASS_TYPE_ID;
            uint32 count = 0;
        };


        const jarray<const char*> m_VulkanDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkInstance m_VulkanInstance = nullptr;
#if JDEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
#endif
        VkPhysicalDevice m_PhysicalDevice = nullptr;
        VkDevice m_Device = nullptr;
        VmaAllocator_T* m_Allocator = nullptr;

        jmap<VulkanQueueType, uint32> m_QueueFamilyIndices;
        jmap<uint32, jshared_ptr<VulkanQueue>> m_Queues;
        jmap<VulkanQueueType, VulkanCommandPool*> m_CommandPools;

        jmap<jstringID, VertexDescription_Vulkan> m_RegisteredVertexTypes;

        juid<render_pass_id_type> m_RenderPassTypeIDs;
        jmap<VulkanRenderPassDescription, RenderPassTypeContainer, VulkanRenderPassDescription::CompatiblePred> m_RenderPassTypes;
        jmap<VulkanRenderPassDescription, VulkanRenderPass*, VulkanRenderPassDescription::EqualPred> m_RenderPasses;


        static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

        bool initVulkan();
        bool createVulkanInstance();
        jarray<const char*> getRequiredVulkanExtensions() const;

        bool pickPhysicalDevice();
        static bool getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, jmap<VulkanQueueType, uint32>& outQueueIndices);
        bool createDevice();
        bool createCommandPools();

        void clearVulkan();
    };
}

#endif
