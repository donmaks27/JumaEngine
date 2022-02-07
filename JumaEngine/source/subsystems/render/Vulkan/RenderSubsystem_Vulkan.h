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
#include "vulkanObjects/VulkanQueueType.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VertexBufferDataBase;
    struct VertexComponentDescription;
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
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem_Vulkan, RenderSubsystem)

    public:
        RenderSubsystem_Vulkan() = default;
        virtual ~RenderSubsystem_Vulkan() override = default;

        VkInstance getVulkanInstance() const { return m_VulkanInstance; }
        VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        VkDevice getDevice() const { return m_Device; }
        VmaAllocator getAllocator() const { return m_Allocator; }

        uint32 getQueueFamilyIndex(const VulkanQueueType queueType) const { return m_QueueFamilyIndices[queueType]; }
        const jshared_ptr<VulkanQueue>& getQueue(const VulkanQueueType queueType) const { return m_Queues[m_QueueFamilyIndices[queueType]]; }
        const jshared_ptr<VulkanCommandPool>& getCommandPool(const VulkanQueueType queueType) const { return m_CommandPools[queueType]; }
        const jshared_ptr<VulkanSwapchain>& getSwapchain() const { return m_Swapchain; }

        virtual void render() override;

        virtual void onEnginePreTerminate() override;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<VulkanContextObjectBase, T>)>
        T* createVulkanObject() { return registerVulkanObject(new T()); }
        virtual jshared_ptr<VertexBuffer> createVertexBuffer() override;
        virtual jshared_ptr<Shader> createShader() override;
        virtual jshared_ptr<Material> createMaterial() override;
        virtual jshared_ptr<Image> createImage() override;
        virtual jshared_ptr<RenderPrimitive> createRenderPrimitive() override;

        void cacheVertexDescription(const VertexBufferDataBase* vertexBufferData);
        const VertexDescription_Vulkan* findVertexDescription(const jstringID& vertexName) const { return m_RegisteredVertexTypes.find(vertexName); }
        
    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

        virtual jarray<const char*> getRequiredVulkanExtensions() const;

    private:

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
        jmap<VulkanQueueType, jshared_ptr<VulkanCommandPool>> m_CommandPools;
        jshared_ptr<VulkanSwapchain> m_Swapchain = nullptr;

        jmap<jstringID, VertexDescription_Vulkan> m_RegisteredVertexTypes;


        static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

        bool initVulkan();
        bool createVulkanInstance();

        bool pickPhysicalDevice();
        static bool getQueueFamilyIndices(VkPhysicalDevice physicalDevice, WindowDescription* window, jmap<VulkanQueueType, uint32>& outQueueIndices);
        bool createDevice();
        bool createCommandPools();
        bool createSwapchain();

        void clearVulkan();

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<VulkanContextObjectBase, T>)>
        T* registerVulkanObject(T* vulkanContextObject)
        {
            if (vulkanContextObject != nullptr)
            {
                vulkanContextObject->m_RenderSubsystem = this;
            }
            return vulkanContextObject;
        }
    };
}

#endif
