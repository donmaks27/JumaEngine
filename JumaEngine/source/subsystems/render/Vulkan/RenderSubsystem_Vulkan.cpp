// Copyright 2021 Leonov Maksim. All Rights Reserved.

#define VMA_IMPLEMENTATION

#include "RenderSubsystem_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "jutils/jlog.h"
#include "jutils/jset.h"
#include "vulkanObjects/VulkanQueue.h"
#include "vulkanObjects/VulkanCommandPool.h"
#include "vulkanObjects/VulkanSwapchain.h"
#include "engine/Engine.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "subsystems/window/Vulkan/WindowSubsystem_Vulkan.h"
#include "subsystems/window/Vulkan/Window_Vulkan.h"
#include "vulkanObjects/VulkanRenderPass.h"
#include "ShaderObject_Vulkan.h"
#include "MaterialObject_Vulkan.h"
#include "VertexBufferObject_Vulkan.h"
#include "RenderOptions_Vulkan.h"
#include "TextureObject_Vulkan.h"

namespace JumaEngine
{
#if JDEBUG
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        const auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }
#endif

    bool RenderSubsystem_Vulkan::initSubsystemInternal()
    {
        if (!Super::initSubsystemInternal())
        {
            return false;
        }
        if (!initVulkan())
        {
            clearVulkan();
            return false;
        }
        return true;
    }
    void RenderSubsystem_Vulkan::clearSubsystemInternal()
    {
        clearVulkan();
        terminateMainWindow();

        Super::clearSubsystemInternal();
    }

    bool RenderSubsystem_Vulkan::initVulkan()
    {
        if (!createVulkanInstance() || !createMainWindow())
        {
            return false;
        }

        if (!pickPhysicalDevice() || !createDevice() || !createCommandPools())
        {
            return false;
        }

        return cast<Window_Vulkan>(m_MainWindow)->createVulkanSwapchain();
    }
    bool RenderSubsystem_Vulkan::createVulkanInstance()
    {
#ifdef JDEBUG
        const jarray<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        {
            uint32 layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            jarray<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.getData());

            bool layersFounded = true;
            for (const auto& layer : validationLayers)
            {
                bool founded = false;
                for (const auto& availableLayer : availableLayers)
                {
                    if (strcmp(layer, availableLayer.layerName) == 0)
                    {
                        founded = true;
                        break;
                    }
                }
                if (!founded)
                {
                    layersFounded = false;
                    break;
                }
            }
            if (!layersFounded)
            {
                JUMA_LOG(error, JSTR("Failed to find validation layers"));
                return false;
            }
        }
#endif

        jarray<const char*> extensions = getRequiredVulkanExtensions();

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = JSTR("JumaEngine");
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = JSTR("JumaEngine");
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &applicationInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32>(extensions.getSize());
        instanceInfo.ppEnabledExtensionNames = extensions.getData();

#if JDEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugMessangerInfo{};
        debugMessangerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessangerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessangerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessangerInfo.pfnUserCallback = Vulkan_DebugCallback;

        instanceInfo.enabledLayerCount = static_cast<uint32>(validationLayers.getSize());
        instanceInfo.ppEnabledLayerNames = validationLayers.getData();
        instanceInfo.pNext = &debugMessangerInfo;
#else
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
#endif

        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_VulkanInstance);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create vulkan instance. Code ") + TO_JSTR(result));
            return false;
        }

#if JDEBUG
        result = CreateDebugUtilsMessengerEXT(m_VulkanInstance, &debugMessangerInfo, nullptr, &m_DebugMessenger);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create debug messenger. Code ") + TO_JSTR(result));
            return false;
        }
#endif
        return true;
    }
    jarray<const char*> RenderSubsystem_Vulkan::getRequiredVulkanExtensions() const
    {
        const WindowSubsystem_Vulkan* windowSubsystem = cast<WindowSubsystem_Vulkan>(getOwnerEngine()->getWindowSubsystem());
        jarray<const char*> extensions = windowSubsystem->getVulkanInstanceExtensions();
#if JDEBUG
        extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }
    VkBool32 RenderSubsystem_Vulkan::Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: JUMA_LOG(warning, pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: JUMA_LOG(error, pCallbackData->pMessage); break;
    	    
        default: ;
        }
        return VK_FALSE;
    }

    bool RenderSubsystem_Vulkan::pickPhysicalDevice()
    {
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            JUMA_LOG(error, JSTR("Failed to find GPUs with Vulkan support"));
            return false;
        }
        jarray<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.getData());

        VkSurfaceKHR surface = cast<Window_Vulkan>(m_MainWindow)->getVulkanSurface();
        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                continue;
            }

            jmap<VulkanQueueType, uint32> indices;
            if (!getQueueFamilyIndices(device, surface, indices))
            {
                continue;
            }

            uint32 extensionCount;
		    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		    jarray<VkExtensionProperties> availableExtensions(extensionCount);
		    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.getData());

            jarray<const char*> extensions = m_VulkanDeviceExtensions;
            for (const auto& extension : availableExtensions)
            {
                for (int32 index = 0; index < extensions.getSize(); index++)
                {
                    if (strcmp(extensions[index], extension.extensionName) == 0)
                    {
                        extensions.removeAt(index);
                        break;
                    }
                }
                if (extensions.isEmpty())
                {
                    break;
                }
            }
            if (!extensions.isEmpty())
            {
                continue;
            }

            uint32 surfaceFormatCount;
	        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr);
            uint32 surfacePresentModeCount;
	        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &surfacePresentModeCount, nullptr);
            if ((surfaceFormatCount == 0) || (surfacePresentModeCount == 0))
            {
                continue;
            }

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
            if (supportedFeatures.samplerAnisotropy != VK_TRUE)
            {
                continue;
            }

            m_PhysicalDevice = device;
            m_QueueFamilyIndices = indices;
            return true;
        }
        return false;
    }
    bool RenderSubsystem_Vulkan::getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, jmap<VulkanQueueType, uint32>& outQueueIndices)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        jarray<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.getData());

        jmap<VulkanQueueType, uint32> indices;
        for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
        {
            const VkQueueFamilyProperties& properties = queueFamilies[queueFamilyIndex];
            if (!indices.contains(VulkanQueueType::Graphics) && (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                indices[VulkanQueueType::Graphics] = queueFamilyIndex;
            }
            if (!indices.contains(VulkanQueueType::Present))
            {
                VkBool32 presentSupport = false;
    		    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &presentSupport);
                if (presentSupport)
                {
                    indices[VulkanQueueType::Present] = queueFamilyIndex;
                }
            }
            if (!indices.contains(VulkanQueueType::Transfer) && (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                indices[VulkanQueueType::Transfer] = queueFamilyIndex;
            }
            if (indices.getSize() == 3)
            {
                outQueueIndices = indices;
                return true;
            }
        }

        return false;
    }
    bool RenderSubsystem_Vulkan::createDevice()
    {
        jset<uint32> uniqueQueueFamilies;
        for (const auto& queueTypeAndIndex : m_QueueFamilyIndices)
        {
            uniqueQueueFamilies.add(queueTypeAndIndex.value);
        }

        float queuePriority = 1.0f;
        jarray<VkDeviceQueueCreateInfo> queueInfos;
        for (const auto& queueFamilyIndex : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		    queueInfo.queueFamilyIndex = queueFamilyIndex;
		    queueInfo.queueCount = 1;
		    queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.add(queueInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        VkDeviceCreateInfo deviceInfo{};
	    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	    deviceInfo.queueCreateInfoCount = static_cast<uint32>(queueInfos.getSize());
	    deviceInfo.pQueueCreateInfos = queueInfos.getData();
	    deviceInfo.pEnabledFeatures = &deviceFeatures;
	    deviceInfo.enabledExtensionCount = static_cast<uint32>(m_VulkanDeviceExtensions.getSize());
	    deviceInfo.ppEnabledExtensionNames = m_VulkanDeviceExtensions.getData();
	    deviceInfo.enabledLayerCount = 0;
        VkResult result = vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create vulkan device. Code ") + TO_JSTR(result));
            return false;
        }

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
        allocatorInfo.instance = m_VulkanInstance;
        allocatorInfo.physicalDevice = m_PhysicalDevice;
        allocatorInfo.device = m_Device;
        result = vmaCreateAllocator(&allocatorInfo, &m_Allocator);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create VMA allocator. Code ") + TO_JSTR(result));
            return false;
        }

        for (const auto& queueFamilyIndex : uniqueQueueFamilies)
        {
            jshared_ptr<VulkanQueue> queue = createVulkanObject<VulkanQueue>();
            if (!queue->init(queueFamilyIndex, 0))
            {
                return false;
            }
            m_Queues[queueFamilyIndex] = queue;
        }
        return true;
    }
    bool RenderSubsystem_Vulkan::createCommandPools()
    {
        VulkanCommandPool* graphicsCommandPool = createVulkanObject<VulkanCommandPool>();
        VulkanCommandPool* transferCommandPool = createVulkanObject<VulkanCommandPool>();
        if (!graphicsCommandPool->init(VulkanQueueType::Graphics) ||
            !transferCommandPool->init(VulkanQueueType::Transfer, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
        {
            JUMA_LOG(error, JSTR("Failed to create command pools"));
            return false;
        }
        m_CommandPools = {
            { VulkanQueueType::Graphics, graphicsCommandPool },
            { VulkanQueueType::Transfer, transferCommandPool },
            { VulkanQueueType::Present, nullptr }
        };
        return true;
    }

    bool RenderSubsystem_Vulkan::pickDepthFormat(VkFormat& outFormat) const
    {
        for (const auto& format : { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT })
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);
            if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                outFormat = format;
                return true;
            }
        }
        return false;
    }

    void RenderSubsystem_Vulkan::clearVulkan()
    {
        if (m_VulkanInstance != nullptr)
        {
            if (m_Device != nullptr)
            {
                vkDeviceWaitIdle(m_Device);

                cast<Window_Vulkan>(m_MainWindow)->destroyVulkanSwapchain();
                for (const auto& renderPassTypeAndPointer : m_RenderPasses)
                {
                    delete renderPassTypeAndPointer.value;
                }
                m_RenderPasses.clear();
                m_RenderPassTypes.clear();

                for (const auto& commandPool : m_CommandPools)
                {
                    delete commandPool.value;
                }
                m_CommandPools.clear();
                m_QueueFamilyIndices.clear();
                m_Queues.clear();

                vmaDestroyAllocator(m_Allocator);
                m_Allocator = nullptr;

                vkDestroyDevice(m_Device, nullptr);
                m_Device = nullptr;
            }

            m_PhysicalDevice = nullptr;

            terminateMainWindow();

#if JDEBUG
            DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
#endif
            vkDestroyInstance(m_VulkanInstance, nullptr);
            m_VulkanInstance = nullptr;
        }
    }

    VulkanSwapchain* RenderSubsystem_Vulkan::getSwapchain() const
    {
        return cast<Window_Vulkan>(m_MainWindow)->getVulkanSwapchain();
    }

    void RenderSubsystem_Vulkan::render()
    {
        m_MainWindow->startRender();

        RenderOptions_Vulkan options;
        VulkanSwapchain* swapchain = cast<Window_Vulkan>(m_MainWindow)->getVulkanSwapchain();
        if (swapchain->startRender(&options))
        {
            callEngineRender(&options);
            swapchain->finishRender(&options);
        }
        if (swapchain->isNeedToRecreate())
        {
            swapchain->applySettings(true);
        }

        m_MainWindow->finishRender();
    }

    void RenderSubsystem_Vulkan::onEnginePreTerminate()
    {
        vkQueueWaitIdle(getQueue(VulkanQueueType::Graphics)->get());
    }

    ShaderObject* RenderSubsystem_Vulkan::createShaderObject()
    {
        return createVulkanObject<ShaderObject_Vulkan>();
    }
    MaterialObject* RenderSubsystem_Vulkan::createMaterialObject()
    {
        return createVulkanObject<MaterialObject_Vulkan>();
    }
    VertexBufferObject* RenderSubsystem_Vulkan::createVertexBufferObject()
    {
        return createVulkanObject<VertexBufferObject_Vulkan>();
    }
    TextureObject* RenderSubsystem_Vulkan::createTextureObject()
    {
        return createVulkanObject<TextureObject_Vulkan>();
    }

    VertexDescription_Vulkan::VertexDescription_Vulkan(const uint32 vertexSize, const jarray<VertexComponentDescription>& vertexComponents)
    {
        binding.binding = 0;
        binding.stride = vertexSize;
        binding.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

        for (int32 index = 0, componentsCount = vertexComponents.getSize(); index < componentsCount; index++)
        {
            const VertexComponentDescription& componentDescriprion = vertexComponents[index];
            VkFormat format = VkFormat::VK_FORMAT_UNDEFINED;
            switch (componentDescriprion.type)
            {
            case VertexComponentType::Float:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case VertexComponentType::Vec2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case VertexComponentType::Vec3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case VertexComponentType::Vec4:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;

            default: continue;
            }

            VkVertexInputAttributeDescription attribute;
            attribute.location = componentDescriprion.ID;
            attribute.binding = binding.binding;
            attribute.offset = componentDescriprion.offset;
            attribute.format = format;
            attributes.add(attribute);
        }
    }

    void RenderSubsystem_Vulkan::registerVertexType(const VertexBufferDataBase* vertexBufferData)
    {
        if (vertexBufferData != nullptr)
        {
            VertexDescription_Vulkan& description = m_RegisteredVertexTypes[vertexBufferData->getVertexName()];
            if (description.attributes.isEmpty())
            {
                description = VertexDescription_Vulkan(vertexBufferData->getVertexSize(), vertexBufferData->getVertexComponents());
            }
        }
    }

    VulkanRenderPass* RenderSubsystem_Vulkan::createRenderPass(const VulkanRenderPassDescription& description)
    {
        if ((description.colorFormat == VK_FORMAT_UNDEFINED) || (description.depthFormat == VK_FORMAT_UNDEFINED))
        {
            return nullptr;
        }

        RenderPassTypeContainer& typeContainer = m_RenderPassTypes[description];
        if (typeContainer.id == INVALID_RENDER_PASS_TYPE_ID)
        {
            typeContainer.id = m_RenderPassTypeIDs.getUID();
        }

        VulkanRenderPass*& renderPass = m_RenderPasses[description];
        if (renderPass == nullptr)
        {
            renderPass = createVulkanObject<VulkanRenderPass>();
            if (!renderPass->init(description, typeContainer.id))
            {
                delete renderPass;
                m_RenderPasses.remove(description);
                return nullptr;
            }
            typeContainer.count++;
        }
        return renderPass;
    }
}

#endif
