// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderSubsystem.h"

#include "vma_lib.h"
#include <vulkan/vulkan_core.h>

#include "utils/jarray.h"
#include "utils/jmap.h"
#include "VulkanQueueType.h"

namespace JumaEngine
{
    class VulkanCommandPool;
    class VulkanContextObject;
    class VulkanQueue;

    class RenderSubsystem_Vulkan : public RenderSubsystem
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderSubsystem_Vulkan, RenderSubsystem)

    public:
        RenderSubsystem_Vulkan() = default;
        virtual ~RenderSubsystem_Vulkan() override = default;

        virtual void render(const RenderQuery& query) override;
        
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<VulkanContextObject, T>)>
        T* createVulkanObject()
        {
            T* object = new T();
            object->m_RenderSubsystem = this;
            return object;
        }

        VkInstance getVulkanInstance() const { return m_VulkanInstance; }
        VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
        VkDevice getDevice() const { return m_Device; }
        VmaAllocator getAllocator() const { return m_Allocator; }

        uint32 getQueueFamilyIndex(const VulkanQueueType queueType) const { return m_QueueFamilyIndices[queueType]; }
        const jshared_ptr<VulkanQueue>& getQueue(const VulkanQueueType queueType) const { return m_Queues[m_QueueFamilyIndices[queueType]]; }

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


        static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

        bool initVulkan();
        bool createVulkanInstance();

        bool pickPhysicalDevice();
        static bool getQueueFamilyIndices(VkPhysicalDevice physicalDevice, const WindowDescription* window, jmap<VulkanQueueType, uint32>& outQueueIndices);
        bool createDevice();
        bool createCommandPools();

        void clearVulkan();
    };
}

#endif
