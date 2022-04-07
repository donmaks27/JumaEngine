// Copyright 2021 Leonov Maksim. All Rights Reserved.

#define VMA_IMPLEMENTATION

#include "RenderSubsystem_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "MaterialRenderAPIObject_Vulkan.h"
#include "RenderOptions_Vulkan.h"
#include "ShaderRenderAPIObject_Vulkan.h"
#include "TextureRenderAPIObject_Vulkan.h"
#include "VertexBufferRenderAPIObject_Vulkan.h"
#include "RenderPipelineRenderAPIObject_Vulkan.h"
#include "RenderTargetRenderAPIObject_Vulkan.h"
#include "engine/Engine.h"
#include "jutils/jset.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "subsystems/window/Vulkan/WindowSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanCommandPool.h"
#include "vulkanObjects/VulkanQueue.h"
#include "vulkanObjects/VulkanRenderPass.h"
#include "vulkanObjects/VulkanSwapchain.h"

#if JDEBUG
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) 
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif

namespace JumaEngine
{
    RenderSubsystem_RenderAPIObject_Vulkan::~RenderSubsystem_RenderAPIObject_Vulkan()
    {
        clearVulkan();
    }

    bool RenderSubsystem_RenderAPIObject_Vulkan::initInternal()
    {
        if (!Super::initInternal())
        {
            return false;
        }
        if (!initVulkan())
        {
            JUMA_LOG(error, JSTR("Failed to initialize render subsystem vulkan object"));
            clearVulkan();
            return false;
        }
        return true;
    }
    bool RenderSubsystem_RenderAPIObject_Vulkan::initVulkan()
    {
        if (!createVulkanInstance() || !createMainWindow())
        {
            return false;
        }
        if (!pickPhysicalDevice() || !createDevice() || !createCommandPools())
        {
            return false;
        }

        const window_id_type mainWindowID = m_Parent->getMainWindowID();
        WindowSubsystem_RenderAPIObject_Vulkan* windowRenderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
        if (!windowRenderObject->createVulkanSwapchain(mainWindowID))
        {
            return false;
        }

        const VulkanSwapchain* swapchain = windowRenderObject->getVulkanSwapchain(mainWindowID);
        m_RenderFrameCount = static_cast<int8>(math::clamp(swapchain->getImageCount() - 1, 1, getMaxRenderFrameCount()));
        if (!windowRenderObject->createRenderImage(mainWindowID))
        {
            return false;
        }

        m_Parent->getRenderPipeline()->createRenderAPIObject();
        return true;
    }
    void RenderSubsystem_RenderAPIObject_Vulkan::clearVulkan()
    {
        clearData();

        const window_id_type mainWindowID = m_Parent->getMainWindowID();

        if (m_VulkanInstance != nullptr)
        {
            if (m_Device != nullptr)
            {
                vkDeviceWaitIdle(m_Device);

                WindowSubsystem_RenderAPIObject_Vulkan* windowRenderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
                if (windowRenderObject != nullptr)
                {
                    windowRenderObject->destroyVulkanSwapchain(mainWindowID);
                    windowRenderObject->destroyRenderImage(mainWindowID);
                }

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
                for (const auto& queue : m_Queues)
                {
                    delete queue.value;
                }
                m_Queues.clear();

                vmaDestroyAllocator(m_Allocator);
                m_Allocator = nullptr;

                vkDestroyDevice(m_Device, nullptr);
                m_Device = nullptr;
            }

            m_PhysicalDevice = nullptr;

            destroyMainWindow();

#if JDEBUG
            DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
#endif
            vkDestroyInstance(m_VulkanInstance, nullptr);
            m_VulkanInstance = nullptr;
        }
    }

    bool RenderSubsystem_RenderAPIObject_Vulkan::createVulkanInstance()
    {
#ifdef JDEBUG
        const jarray<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        {
            uint32 layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            jarray<VkLayerProperties> availableLayers(static_cast<int32>(layerCount));
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

        VkDebugUtilsMessengerCreateInfoEXT debugMessangerInfo{};
        debugMessangerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessangerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessangerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessangerInfo.pfnUserCallback = RenderSubsystem_RenderAPIObject_Vulkan::Vulkan_DebugCallback;
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
        instanceInfo.enabledLayerCount = static_cast<uint32>(validationLayers.getSize());
        instanceInfo.ppEnabledLayerNames = validationLayers.getData();
        instanceInfo.pNext = &debugMessangerInfo;
#else
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
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
    jarray<const char*> RenderSubsystem_RenderAPIObject_Vulkan::getRequiredVulkanExtensions() const
    {
        WindowSubsystem_RenderAPIObject_Vulkan* windowRenderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
        if (windowRenderObject == nullptr)
        {
            return {};
        }

        jarray<const char*> extensions = windowRenderObject->getVulkanInstanceExtensions();
#if JDEBUG
        extensions.add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        return extensions;
    }
#if JDEBUG
    VkBool32 RenderSubsystem_RenderAPIObject_Vulkan::Vulkan_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
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
#endif

    bool RenderSubsystem_RenderAPIObject_Vulkan::pickPhysicalDevice()
    {
        const window_id_type mainWindowID = m_Parent->getMainWindowID();

        WindowSubsystem_RenderAPIObject_Vulkan* windowRenderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
        VkSurfaceKHR surface = windowRenderObject != nullptr ? windowRenderObject->getWindowVulkanSurface(mainWindowID) : nullptr;
        if (surface == nullptr)
        {
            JUMA_LOG(error, JSTR("Can't get window's surface"));
            return false;
        }

        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            JUMA_LOG(error, JSTR("Failed to find GPUs with Vulkan support"));
            return false;
        }
        jarray<VkPhysicalDevice> devices(static_cast<int32>(deviceCount));
        vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.getData());

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
		    jarray<VkExtensionProperties> availableExtensions(static_cast<int32>(extensionCount));
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
            windowRenderObject->updateSupportedPresentModes(mainWindowID);
            return true;
        }
        return false;
    }
    bool RenderSubsystem_RenderAPIObject_Vulkan::getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, 
        jmap<VulkanQueueType, uint32>& outQueueIndices)
    {
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        jarray<VkQueueFamilyProperties> queueFamilies(static_cast<int32>(queueFamilyCount));
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.getData());

        jmap<VulkanQueueType, uint32> indices;
        for (int32 queueFamilyIndex = 0, size = static_cast<int32>(queueFamilyCount); queueFamilyIndex < size; queueFamilyIndex++)
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

    bool RenderSubsystem_RenderAPIObject_Vulkan::createDevice()
    {
        jset<uint32> uniqueQueueFamilies;
        for (const auto& queueTypeAndIndex : m_QueueFamilyIndices)
        {
            uniqueQueueFamilies.add(queueTypeAndIndex.value);
        }

        constexpr float queuePriority = 1.0f;
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
            VulkanQueue* queue = createVulkanObject<VulkanQueue>();
            if (!queue->init(queueFamilyIndex, 0))
            {
                delete queue;
                return false;
            }
            m_Queues[queueFamilyIndex] = queue;
        }
        return true;
    }
    bool RenderSubsystem_RenderAPIObject_Vulkan::createCommandPools()
    {
        VulkanCommandPool* graphicsCommandPool = createVulkanObject<VulkanCommandPool>();
        VulkanCommandPool* transferCommandPool = createVulkanObject<VulkanCommandPool>();
        if (!graphicsCommandPool->init(VulkanQueueType::Graphics) ||
            !transferCommandPool->init(VulkanQueueType::Transfer, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT))
        {
            JUMA_LOG(error, JSTR("Failed to create command pools"));
            delete graphicsCommandPool;
            delete transferCommandPool;
            return false;
        }
        m_CommandPools = {
            { VulkanQueueType::Graphics, graphicsCommandPool },
            { VulkanQueueType::Transfer, transferCommandPool },
            { VulkanQueueType::Present, nullptr }
        };
        return true;
    }

    bool RenderSubsystem_RenderAPIObject_Vulkan::pickDepthFormat(VkFormat& outFormat) const
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

    ShaderRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createShaderObject()
    {
        return createVulkanObject<ShaderRenderAPIObject_Vulkan>();
    }
    MaterialRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createMaterialObject()
    {
        return createVulkanObject<MaterialRenderAPIObject_Vulkan>();
    }
    VertexBufferRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createVertexBufferObject()
    {
        return createVulkanObject<VertexBufferRenderAPIObject_Vulkan>();
    }
    TextureRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createTextureObject()
    {
        return createVulkanObject<TextureRenderAPIObject_Vulkan>();
    }
    RenderTargetRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createRenderTargetObject()
    {
        return createVulkanObject<RenderTargetRenderAPIObject_Vulkan>();
    }
    RenderPipelineRenderAPIObject* RenderSubsystem_RenderAPIObject_Vulkan::createRenderPipelineObject()
    {
        return createVulkanObject<RenderPipelineRenderAPIObject_Vulkan>();
    }

    void RenderSubsystem_RenderAPIObject_Vulkan::render()
    {
        WindowSubsystem* windowSubsystem = m_Parent->getOwnerEngine()->getWindowSubsystem();

        windowSubsystem->startRender();
        m_Parent->getRenderPipeline()->render();
        windowSubsystem->finishRender();
    }
    void RenderSubsystem_RenderAPIObject_Vulkan::waitForRenderFinish()
    {
        vkQueueWaitIdle(getQueue(VulkanQueueType::Graphics)->get());

        Super::waitForRenderFinish();
    }

    void RenderSubsystem_RenderAPIObject_Vulkan::registerVertexType(const VertexBufferDataBase* vertexBufferData)
    {
        if (vertexBufferData == nullptr)
        {
            return;
        }

        VertexDescription_Vulkan& description = m_RegisteredVertexTypes[vertexBufferData->getVertexName()];
        if (!description.attributes.isEmpty())
        {
            return;
        }

        description.binding.binding = 0;
        description.binding.stride = vertexBufferData->getVertexSize();
        description.binding.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

        const jarray<VertexComponentDescription> vertexComponents = vertexBufferData->getVertexComponents();
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
            attribute.binding = description.binding.binding;
            attribute.offset = componentDescriprion.offset;
            attribute.format = format;
            description.attributes.add(attribute);
        }
    }

    VulkanRenderPass* RenderSubsystem_RenderAPIObject_Vulkan::createRenderPass(const VulkanRenderPassDescription& description)
    {
        if ((description.colorFormat == VK_FORMAT_UNDEFINED) || (description.depthFormat == VK_FORMAT_UNDEFINED))
        {
            return nullptr;
        }

        render_pass_id_type& renderPassID = m_RenderPassTypes[description];
        if (renderPassID == INVALID_RENDER_PASS_TYPE_ID)
        {
            renderPassID = m_RenderPassTypeIDs.getUID();
        }

        VulkanRenderPass*& renderPass = m_RenderPasses[description];
        if (renderPass == nullptr)
        {
            renderPass = createVulkanObject<VulkanRenderPass>();
            if (!renderPass->init(description, renderPassID))
            {
                JUMA_LOG(error, JSTR("Failed to create render pass"));
                delete renderPass;
                m_RenderPasses.remove(description);
                return nullptr;
            }
        }
        return renderPass;
    }
}

#endif
