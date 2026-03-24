#include "VulkanBase.hpp"
#include "../util/Defines.hpp"
#include "../window/Window.hpp"
#include <cstdint>
#include <cstring>
#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

VulkanBase::~VulkanBase() {
    if (!instance) {
        return; // Instance was never created, nothing to clean up
    }

    if (device) {
        waitForDeviceIdle();
        if (swapchain) {
            executeCallbacks(destroySwapchainCallbacks);
            for (auto& imageView : swapchainImageViews) {
                if (imageView) {
                    vkDestroyImageView(device, imageView, nullptr);
                }
            }
            vkDestroySwapchainKHR(device, swapchain, nullptr);
        }
        executeCallbacks(destroyDeviceCallbacks);
        vkDestroyDevice(device, nullptr);
    }

    if (surface) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    if (debugMessenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (vkDestroyDebugUtilsMessenger) {
            vkDestroyDebugUtilsMessenger(instance, debugMessenger, nullptr);
        }
    }

    vkDestroyInstance(instance, nullptr);
}

inline void VulkanBase::addLayerOrExtension(std::vector<const char*>& container, const char* name) {
    if (!name) {
        return;
    }

    for (auto& item : container) {
        if (std::strcmp(item, name) == 0) {
            return; // Already exists, do not add again
        }
    }
    container.push_back(name);
}

Result VulkanBase::createDebugMessenger() {
    static PFN_vkDebugUtilsMessengerCallbackEXT debugUtilsMessengerCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                                 void* pUserData) {
        std::cout << std::format("{}\n\n", pCallbackData->pMessage);
        return VK_FALSE; // Return false to indicate that the Vulkan call that triggered the validation message should not be aborted
    };

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugUtilsMessengerCallback,
    };

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkCreateDebugUtilsMessenger) {
        Result result = vkCreateDebugUtilsMessenger(this->instance, &debugUtilsMessengerCreateInfo, nullptr, &this->debugMessenger);
        if (result != VK_SUCCESS) {
            std::cerr << "Failed to create Vulkan debug messenger: " << string_VkResult(result) << std::endl;
        }
        return result;
    }
    std::cerr << "Failed to get function pointer for vkCreateDebugUtilsMessengerEXT, debug messenger will not be created." << std::endl;
    return VK_RESULT_MAX_ENUM; // Return an error code to indicate that the debug messenger was not created
}

Result VulkanBase::createSwapchainInternal() {
    if (Result result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain); result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan swapchain: " << string_VkResult(result) << std::endl;
        return result;
    }

    uint32_t imageCount = 0;
    if (Result result = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of swapchain images: " << string_VkResult(result) << std::endl;
        return result;
    }
    swapchainImages.resize(imageCount);
    if (Result result = vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()); result != VK_SUCCESS) {
        std::cerr << "Failed to get swapchain images: " << string_VkResult(result) << std::endl;
        return result;
    }

    swapchainImageViews.resize(imageCount);
    VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchainCreateInfo.imageFormat,
        .components{
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange{
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1,
        },
    };

    for (size_t i = 0; i < imageCount; i++) {
        imageViewCreateInfo.image = swapchainImages[i];
        if (Result result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]); result != VK_SUCCESS) {
            std::cerr << "Failed to create image view for swapchain image at index " << i << ": " << string_VkResult(result) << std::endl;
            return result;
        }
    }

    return VK_SUCCESS;
}

void VulkanBase::executeCallbacks(const std::vector<std::function<void()>>& callbacks) {
    for (const auto& callback : callbacks) {
        if (callback) {
            callback();
        }
    }
}

VulkanBase& VulkanBase::getVulkanBase() {
    static VulkanBase instance;
    return instance;
}

uint32_t VulkanBase::getApiVersion() const { return this->apiVersion; }
Result VulkanBase::setApiVersionToLatest() {
    if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")) {
        return vkEnumerateInstanceVersion(&apiVersion);
    }
    return VK_SUCCESS;
}

Result VulkanBase::createInstance(VkInstanceCreateFlags flags) {
    if constexpr (ENABLE_DEBUG_MESSENGER) {
        addInstanceLayer("VK_LAYER_KHRONOS_validation");
        addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = this->apiVersion,
    };

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = flags,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(instanceLayers.size()),
        .ppEnabledLayerNames = instanceLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
        .ppEnabledExtensionNames = instanceExtensions.data(),
    };

    if (Result result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance: " << string_VkResult(result) << std::endl;
        return result;
    }

    std::cout << std::format("Vulkan instance created with API version {}.{}.{}", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion)) << std::endl;

    if constexpr (ENABLE_DEBUG_MESSENGER) {
        createDebugMessenger();
    }

    return VK_SUCCESS;
}

VkInstance VulkanBase::getInstance() const { return this->instance; }

void VulkanBase::addInstanceLayer(const char* layerName) { addLayerOrExtension(this->instanceLayers, layerName); }
void VulkanBase::addInstanceExtension(const char* extensionName) { addLayerOrExtension(this->instanceExtensions, extensionName); }
void VulkanBase::addDeviceExtension(const char* extensionName) { addLayerOrExtension(this->deviceExtensions, extensionName); }

void VulkanBase::setInstanceLayers(std::span<const char*> layers) { this->instanceLayers.assign_range(layers); }
void VulkanBase::setInstanceExtensions(std::span<const char*> extensions) { this->instanceExtensions.assign_range(extensions); }
void VulkanBase::setDeviceExtensions(std::span<const char*> extensions) { this->deviceExtensions.assign_range(extensions); }

const std::vector<const char*>& VulkanBase::getInstanceLayers() const { return this->instanceLayers; }
const std::vector<const char*>& VulkanBase::getInstanceExtensions() const { return this->instanceExtensions; }
const std::vector<const char*>& VulkanBase::getDeviceExtensions() const { return this->deviceExtensions; }

Result VulkanBase::checkInstanceLayers(std::span<const char*> layersToCheck) const {
    uint32_t layerCount = 0;
    std::vector<VkLayerProperties> availableLayers;
    if (Result result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of instance layers: " << string_VkResult(result) << std::endl;
        return result;
    }

    if (layerCount > 0) {
        availableLayers.resize(layerCount);
        if (Result result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); result != VK_SUCCESS) {
            std::cerr << "Failed to enumerate instance layer properties: " << string_VkResult(result) << std::endl;
            return result;
        }

        for (auto& i : layersToCheck) {
            bool found = false;
            for (auto& j : availableLayers) {
                if (std::strcmp(i, j.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "Instance layer not found: " << i << std::endl;
                i = nullptr; // Mark as not found
            }
        }
    } else {
        for (auto& i : layersToCheck) {
            std::cerr << "No instance layers available, cannot find layer: " << i << std::endl;
            i = nullptr; // Mark as not found
        }
    }

    return VK_SUCCESS;
}

Result VulkanBase::checkInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> availableExtensions;
    if (Result result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of instance extensions for layer " << (layerName ? layerName : "global") << ": " << string_VkResult(result) << std::endl;
        return result;
    }

    if (extensionCount > 0) {
        availableExtensions.resize(extensionCount);
        if (Result result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, availableExtensions.data()); result != VK_SUCCESS) {
            std::cerr << "Failed to enumerate instance extension properties for layer " << (layerName ? layerName : "global") << ": " << string_VkResult(result) << std::endl;
            return result;
        }

        for (auto& i : extensionsToCheck) {
            bool found = false;
            for (auto& j : availableExtensions) {
                if (std::strcmp(i, j.extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "Instance extension not found: " << i << " for layer " << (layerName ? layerName : "global") << std::endl;
                i = nullptr; // Mark as not found
            }
        }
    } else {
        for (auto& i : extensionsToCheck) {
            std::cerr << "No instance extensions available for layer " << (layerName ? layerName : "global") << ", cannot find extension: " << i << std::endl;
            i = nullptr; // Mark as not found
        }
    }

    return VK_SUCCESS;
}

Result VulkanBase::checkDeviceExtensions(std::span<const char*> extensionsToCheck) const {
    // TODO: Implement device extension availability check using vkEnumerateDeviceExtensionProperties for the current physical
    // device
    return VK_SUCCESS;
}

VkPhysicalDevice VulkanBase::getCurrentPhysicalDevice() const { return this->physicalDevice; }
const VkPhysicalDeviceProperties& VulkanBase::getPhysicalDeviceProperties() const { return this->physicalDeviceProperties; }
const VkPhysicalDeviceMemoryProperties& VulkanBase::getPhysicalDeviceMemoryProperties() const { return this->physicalDeviceMemoryProperties; }

VkPhysicalDevice VulkanBase::getAvailablePhysicalDevice(uint32_t index) const {
    if (index >= this->availablePhysicalDevices.size()) {
        return VK_NULL_HANDLE; // Invalid index
    }
    return this->availablePhysicalDevices[index];
}

const std::vector<VkPhysicalDevice>& VulkanBase::getAvailablePhysicalDevices() const { return this->availablePhysicalDevices; }
uint32_t VulkanBase::getAvailablePhysicalDeviceCount() const { return static_cast<uint32_t>(this->availablePhysicalDevices.size()); }

Result VulkanBase::findAvailablePhysicalDevices() {
    uint32_t deviceCount = 0;
    if (Result result = vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of physical devices: " << string_VkResult(result);
        return result;
    }

    if (!deviceCount) {
        std::cerr << "No physical devices found that support Vulkan." << std::endl;
        abort(); // No devices found, cannot continue
    }

    availablePhysicalDevices.resize(deviceCount);
    if (Result result = vkEnumeratePhysicalDevices(this->instance, &deviceCount, availablePhysicalDevices.data()); result != VK_SUCCESS) {
        std::cerr << "Failed to enumerate physical devices: " << string_VkResult(result);
        return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::determinePhysicalDevice(uint32_t deviceIndex, bool enableGraphicsQueue, bool enableComputeQueue) {
    static constexpr uint32_t notFound = INT32_MAX;

    struct QueueFamilyIndexCombination {
        uint32_t graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t compute = VK_QUEUE_FAMILY_IGNORED;
    };
    static std::vector<QueueFamilyIndexCombination> queueFamilyIndexCombinations(availablePhysicalDevices.size());
    auto& [graphicsIndex, presentationIndex, computeIndex] = queueFamilyIndexCombinations[deviceIndex];

    if (graphicsIndex == notFound && enableGraphicsQueue || presentationIndex == notFound && surface || computeIndex == notFound && enableComputeQueue) {
        return VK_RESULT_MAX_ENUM; // Return an error code to indicate that required queue families were not found for this device
    }

    if (graphicsIndex == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue || presentationIndex == VK_QUEUE_FAMILY_IGNORED && surface || computeIndex == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue) {
        uint32_t indices[3];
        Result result = getQueueFamilyIndices(availablePhysicalDevices[deviceIndex], enableGraphicsQueue, enableComputeQueue, indices);

        if (result == VK_SUCCESS || result == VK_RESULT_MAX_ENUM) {
            if (enableGraphicsQueue) {
                graphicsIndex = indices[0] & INT32_MAX;
            }
            if (surface) {
                presentationIndex = indices[1] & INT32_MAX;
            }
            if (enableComputeQueue) {
                computeIndex = indices[2] & INT32_MAX;
            }
        }

        if (result != VK_SUCCESS) {
            std::cerr << "Failed to determine queue family indices for physical device at index " << deviceIndex << ": " << string_VkResult(result) << std::endl;
            return result;
        }
    } else {
        queueFamilyIndexGraphics = enableGraphicsQueue ? graphicsIndex : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndexPresentation = surface ? presentationIndex : VK_QUEUE_FAMILY_IGNORED;
        queueFamilyIndexCompute = enableComputeQueue ? computeIndex : VK_QUEUE_FAMILY_IGNORED;
    }

    physicalDevice = availablePhysicalDevices[deviceIndex];

    return VK_SUCCESS;
}

Result VulkanBase::createDevice(VkDeviceCreateFlags flags) {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfos[3]{
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        },
    };
    uint32_t queueCreateInfoCount = 0;
    if (queueFamilyIndexGraphics != VK_QUEUE_FAMILY_IGNORED) {
        queueCreateInfos[queueCreateInfoCount].queueFamilyIndex = queueFamilyIndexGraphics;
        queueCreateInfoCount++;
    }
    if (queueFamilyIndexPresentation != VK_QUEUE_FAMILY_IGNORED && queueFamilyIndexPresentation != queueFamilyIndexGraphics) {
        queueCreateInfos[queueCreateInfoCount].queueFamilyIndex = queueFamilyIndexPresentation;
        queueCreateInfoCount++;
    }
    if (queueFamilyIndexCompute != VK_QUEUE_FAMILY_IGNORED && queueFamilyIndexCompute != queueFamilyIndexGraphics && queueFamilyIndexCompute != queueFamilyIndexPresentation) {
        queueCreateInfos[queueCreateInfoCount].queueFamilyIndex = queueFamilyIndexCompute;
        queueCreateInfoCount++;
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .flags = flags,
        .queueCreateInfoCount = queueCreateInfoCount,
        .pQueueCreateInfos = queueCreateInfos,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = &physicalDeviceFeatures,
    };

    if (Result result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan device: " << string_VkResult(result) << std::endl;
        return result;
    }

    if (queueFamilyIndexGraphics != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexGraphics, 0, &graphicsQueue);
    }
    if (queueFamilyIndexPresentation != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexPresentation, 0, &presentationQueue);
    }
    if (queueFamilyIndexCompute != VK_QUEUE_FAMILY_IGNORED) {
        vkGetDeviceQueue(device, queueFamilyIndexCompute, 0, &computeQueue);
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

    std::cout << std::format("Renderer device selected: {} (type: {})\n", physicalDeviceProperties.deviceName, string_VkPhysicalDeviceType(physicalDeviceProperties.deviceType));

    executeCallbacks(createDeviceCallbacks);

    return VK_SUCCESS;
}

VkDevice VulkanBase::getDevice() const { return this->device; }

Result VulkanBase::getQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphics, bool enableCompute, uint32_t (&queueFamilyIndices)[3]) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        std::cerr << "Failed to find any queue families for the current physical device." << std::endl;
        return VK_RESULT_MAX_ENUM; // Return an error code to indicate that no queue families were found
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    auto& [indexGraphics, indexPresentation, indexCompute] = queueFamilyIndices;
    indexGraphics = indexPresentation = indexCompute = VK_QUEUE_FAMILY_IGNORED;

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkBool32 supportGraphics = enableGraphics && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT);
        VkBool32 supportPresentation = VK_FALSE;
        VkBool32 supportCompute = enableCompute && (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT);

        if (surface) {
            if (Result result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation); result != VK_SUCCESS) {
                std::cerr << "Failed to check presentation support for queue family index " << i << ": " << string_VkResult(result) << std::endl;
                return result;
            }
        }

        if (supportGraphics && supportCompute) {
            if (supportPresentation) {
                indexGraphics = indexPresentation = indexCompute = i;
                break; // Found a queue family that supports all required operations, can stop searching
            }

            if (indexGraphics != indexCompute || indexGraphics == VK_QUEUE_FAMILY_IGNORED) {
                indexGraphics = i;
                indexCompute = i;
            }

            if (!surface) {
                break;
            }
        }

        if (supportGraphics && indexGraphics == VK_QUEUE_FAMILY_IGNORED) {
            indexGraphics = i;
        }
        if (supportPresentation && indexPresentation == VK_QUEUE_FAMILY_IGNORED) {
            indexPresentation = i;
        }
        if (supportCompute && indexCompute == VK_QUEUE_FAMILY_IGNORED) {
            indexCompute = i;
        }
    }

    if (indexGraphics == VK_QUEUE_FAMILY_IGNORED && enableGraphics || indexPresentation == VK_QUEUE_FAMILY_IGNORED && surface || indexCompute == VK_QUEUE_FAMILY_IGNORED && enableCompute) {
        return VK_RESULT_MAX_ENUM; // Return an error code to indicate that required queue families were not found
    }

    this->queueFamilyIndexGraphics = indexGraphics;
    this->queueFamilyIndexPresentation = indexPresentation;
    this->queueFamilyIndexCompute = indexCompute;

    return VK_SUCCESS;
}

uint32_t VulkanBase::getQueueFamilyIndexGraphics() const { return this->queueFamilyIndexGraphics; }
uint32_t VulkanBase::getQueueFamilyIndexPresentation() const { return this->queueFamilyIndexPresentation; }
uint32_t VulkanBase::getQueuefamilyIndexCompute() const { return this->queueFamilyIndexCompute; }

VkQueue VulkanBase::getGraphicsQueue() const { return this->graphicsQueue; }
VkQueue VulkanBase::getPresentationQueue() const { return this->presentationQueue; }
VkQueue VulkanBase::getComputeQueue() const { return this->computeQueue; }

VkSurfaceKHR VulkanBase::getSurface() const { return this->surface; }
void VulkanBase::initializeSurface(VkSurfaceKHR surface) {
    if (!this->surface) {
        this->surface = surface;
    }
}

const VkSurfaceFormatKHR& VulkanBase::getAvailableSurfaceFormat(uint32_t index) const { return this->availableSurfaceFormats[index]; }
const VkColorSpaceKHR& VulkanBase::getAvailableSurfaceColorSpace(uint32_t index) const { return this->availableSurfaceFormats[index].colorSpace; }
uint32_t VulkanBase::getAvailableSurfaceFormatCount() const { return static_cast<uint32_t>(this->availableSurfaceFormats.size()); }

VkSwapchainKHR VulkanBase::getSwapchain() const { return this->swapchain; }
VkImage VulkanBase::getSwapchainImage(uint32_t index) const { return this->swapchainImages[index]; }
VkImageView VulkanBase::getSwapchainImageView(uint32_t index) const { return this->swapchainImageViews[index]; }
uint32_t VulkanBase::getSwapchainImageCount() const { return static_cast<uint32_t>(this->swapchainImages.size()); }
const VkSwapchainCreateInfoKHR& VulkanBase::getSwapchainCreateInfo() const { return this->swapchainCreateInfo; }
const std::vector<VkSurfaceFormatKHR>& VulkanBase::getAvailableSurfaceFormats() const { return this->availableSurfaceFormats; }

Result VulkanBase::findAvailableSurfaceFormats() {
    uint32_t surfaceFormatCount = 0;
    if (Result result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of available surface formats: " << string_VkResult(result) << std::endl;
        return result;
    }

    if (surfaceFormatCount == 0) {
        std::cerr << "No surface formats found for the current physical device and surface." << std::endl;
        abort(); // No surface formats found, cannot continue
    }
    availableSurfaceFormats.resize(surfaceFormatCount);
    if (Result result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, availableSurfaceFormats.data()); result != VK_SUCCESS) {
        std::cerr << "Failed to enumerate available surface formats: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::setSurfaceFormat(const VkSurfaceFormatKHR& surfaceFormat) {
    bool available = false;
    if (surfaceFormat.format == VK_FORMAT_UNDEFINED) {
        for (auto& i : availableSurfaceFormats) {
            if (i.colorSpace == surfaceFormat.colorSpace) {
                swapchainCreateInfo.imageFormat = i.format;
                swapchainCreateInfo.imageColorSpace = i.colorSpace;
                available = true;
                break;
            }
        }
    } else {
        for (auto& i : availableSurfaceFormats) {
            if (i.format == surfaceFormat.format && i.colorSpace == surfaceFormat.colorSpace) {
                swapchainCreateInfo.imageFormat = i.format;
                swapchainCreateInfo.imageColorSpace = i.colorSpace;
                available = true;
                break;
            }
        }
    }

    if (!available) {
        std::cerr << "The specified surface format (format: " << string_VkFormat(surfaceFormat.format) << ", color space: " << string_VkColorSpaceKHR(surfaceFormat.colorSpace) << ") is not available for the current physical device and surface."
                  << std::endl;
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }

    if (swapchain) {
        return recreateSwapchain();
    }

    return VK_SUCCESS;
}

Result VulkanBase::createSwapchain(bool limitFrameRate, VkSwapchainCreateFlagsKHR flags) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities{};
    if (Result result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities); result != VK_SUCCESS) {
        std::cerr << "Failed to get surface capabilities: " << string_VkResult(result) << std::endl;
        return result;
    }

    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + (surfaceCapabilities.maxImageCount > surfaceCapabilities.minImageCount ? 1 : 0);

    VkExtent2D defaultExtent{
        .width = glm::clamp(Window::DEFAULT_WINDOW_SIZE.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
        .height = glm::clamp(Window::DEFAULT_WINDOW_SIZE.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height),
    };
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent.width != UINT32_MAX ? surfaceCapabilities.currentExtent : defaultExtent;

    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;

    if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    } else {
        for (size_t i = 0; i < 4; i++) {
            if (surfaceCapabilities.supportedCompositeAlpha & (1 << i)) {
                swapchainCreateInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR(surfaceCapabilities.supportedCompositeAlpha & (1 << i));
                break;
            }
        }
    }

    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    } else {
        std::cerr << "Warning: Surface does not support VK_IMAGE_USAGE_TRANSFER_DST_BIT, which may limit the ability to perform certain operations on swapchain images." << std::endl;
    }

    if (availableSurfaceFormats.empty()) {
        if (Result result = findAvailableSurfaceFormats(); result != VK_SUCCESS) {
            std::cerr << "Failed to find available surface formats: " << string_VkResult(result) << std::endl;
            return result;
        }
    }

    if (swapchainCreateInfo.imageFormat == VK_FORMAT_UNDEFINED) {
        if (setSurfaceFormat({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}) && setSurfaceFormat({VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})) {
            std::cerr << "Warning: Failed to set a suitable surface format for the swapchain." << std::endl;
        }
    }

    uint32_t presentModeCount = 0;
    if (Result result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of available present modes: " << string_VkResult(result) << std::endl;
        return result;
    }
    if (presentModeCount == 0) {
        std::cerr << "No present modes found for the current physical device and surface." << std::endl;
        abort(); // No present modes found, cannot continue
    }
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (Result result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()); result != VK_SUCCESS) {
        std::cerr << "Failed to enumerate available present modes: " << string_VkResult(result) << std::endl;
        return result;
    }

    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Guaranteed to be available
    if (!limitFrameRate) {
        for (size_t i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                swapchainCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
        }
    }

    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.flags = flags;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.clipped = VK_TRUE;

    if (Result result = createSwapchainInternal(); result != VK_SUCCESS) {
        std::cerr << "Failed to create swapchain: " << string_VkResult(result) << std::endl;
        return result;
    }

    executeCallbacks(createSwapchainCallbacks);

    return VK_SUCCESS;
}

Result VulkanBase::recreateSwapchain() {
    VkSurfaceCapabilitiesKHR surfaceCapabilities{};
    if (Result result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities); result != VK_SUCCESS) {
        std::cerr << "Failed to get surface capabilities: " << string_VkResult(result) << std::endl;
        return result;
    }
    if (surfaceCapabilities.currentExtent.width == 0 || surfaceCapabilities.currentExtent.height == 0) {
        std::cerr << "Surface is currently not available for rendering (extent is 0)." << std::endl;
        return VK_SUBOPTIMAL_KHR; // Surface is not available for rendering, but swapchain is still valid and can be recreated when the surface becomes available again
    }
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.oldSwapchain = swapchain;

    Result result = vkQueueWaitIdle(graphicsQueue);
    if (result == VK_SUCCESS && graphicsQueue != presentationQueue) {
        result = vkQueueWaitIdle(presentationQueue);
    }
    if (result != VK_SUCCESS) {
        std::cerr << "Failed to wait for queues to become idle before recreating swapchain: " << string_VkResult(result) << std::endl;
        return result;
    }

    executeCallbacks(destroySwapchainCallbacks);

    for (auto& imageView : swapchainImageViews) {
        if (imageView) {
            vkDestroyImageView(device, imageView, nullptr);
        }
    }
    swapchainImageViews.clear();

    if (result = createSwapchainInternal(); result != VK_SUCCESS) {
        std::cerr << "Failed to recreate swapchain: " << string_VkResult(result) << std::endl;
        return result;
    }

    executeCallbacks(createSwapchainCallbacks);

    return VK_SUCCESS;
}

void VulkanBase::addCreateSwapchainCallback(std::function<void()> callback) { createSwapchainCallbacks.push_back(std::move(callback)); }
void VulkanBase::addDestroySwapchainCallback(std::function<void()> callback) { destroySwapchainCallbacks.push_back(std::move(callback)); }
void VulkanBase::addCreateDeviceCallback(std::function<void()> callback) { createDeviceCallbacks.push_back(std::move(callback)); }
void VulkanBase::addDestroyDeviceCallback(std::function<void()> callback) { destroyDeviceCallbacks.push_back(std::move(callback)); }

Result VulkanBase::waitForDeviceIdle() const {
    if (Result result = vkDeviceWaitIdle(device); result != VK_SUCCESS) {
        std::cerr << "Failed to wait for device to become idle: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::recreateDevice(VkDeviceCreateFlags flags) {
    if (device) {
        if (Result result = waitForDeviceIdle(); result != VK_SUCCESS && result != VK_ERROR_DEVICE_LOST) {
            std::cerr << "Failed to wait for device to become idle before recreating device: " << string_VkResult(result) << std::endl;
            return result;
        }
        if (swapchain) {
            executeCallbacks(destroySwapchainCallbacks);

            for (auto& imageView : swapchainImageViews) {
                if (imageView) {
                    vkDestroyImageView(device, imageView, nullptr);
                }
            }
            swapchainImageViews.clear();
            vkDestroySwapchainKHR(device, swapchain, nullptr);
            swapchain = VK_NULL_HANDLE;
            swapchainCreateInfo = {};
        }
        executeCallbacks(destroyDeviceCallbacks);
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    return createDevice(flags);
}

void VulkanBase::terminate() {
    this->~VulkanBase();

    instance = VK_NULL_HANDLE;
    physicalDevice = VK_NULL_HANDLE;
    device = VK_NULL_HANDLE;
    surface = VK_NULL_HANDLE;
    swapchain = VK_NULL_HANDLE;
    swapchainCreateInfo = {};
    debugMessenger = VK_NULL_HANDLE;
}

uint32_t VulkanBase::getCurrentImageIndex() const { return currentImageIndex; }

Result VulkanBase::swapImage(VkSemaphore imageAvailableSemaphore) {
    if (swapchainCreateInfo.oldSwapchain && swapchainCreateInfo.oldSwapchain != swapchain) {
        vkDestroySwapchainKHR(device, swapchainCreateInfo.oldSwapchain, nullptr);
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    }

    while (Result result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &currentImageIndex)) {
        switch (result) {
            case VK_SUBOPTIMAL_KHR:
            case VK_ERROR_OUT_OF_DATE_KHR:
                if (Result result = recreateSwapchain(); result != VK_SUCCESS) {
                    std::cerr << "Failed to recreate swapchain after it became out of date or suboptimal: " << string_VkResult(result) << std::endl;
                    return result;
                }
                break;
            default:
                std::cerr << "Failed to acquire next image from swapchain: " << string_VkResult(result) << std::endl;
                return result;
        }
    }

    return VK_SUCCESS;
}

Result VulkanBase::submitCommandBufferToGraphicsQueue(VkSubmitInfo& submitInfo, VkFence fence) const {
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    if (Result result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence); result != VK_SUCCESS) {
        std::cerr << "Failed to submit command buffer to graphics queue: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::submitCommandBufferToGraphicsQueue(VkCommandBuffer commandBuffer, VkSemaphore imageAvailableSemaphore, VkSemaphore renderFinishedSemaphore, VkFence fence, VkPipelineStageFlags waitDstStageImageAvailable) const {
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    if (imageAvailableSemaphore) {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &waitDstStageImageAvailable;
    }

    if (renderFinishedSemaphore) {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
    }

    return submitCommandBufferToGraphicsQueue(submitInfo, fence);
}

Result VulkanBase::submitCommandBufferToGraphicsQueue(VkCommandBuffer commandBuffer, VkFence fence) const {
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    return submitCommandBufferToGraphicsQueue(submitInfo, fence);
}

Result VulkanBase::submitCommandBufferToComputeQueue(VkSubmitInfo& submitInfo, VkFence fence) const {
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    if (Result result = vkQueueSubmit(computeQueue, 1, &submitInfo, fence); result != VK_SUCCESS) {
        std::cerr << "Failed to submit command buffer to compute queue: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::submitCommandBufferToComputeQueue(VkCommandBuffer commandBuffer, VkFence fence) const {
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    return submitCommandBufferToComputeQueue(submitInfo, fence);
}

Result VulkanBase::presentImage(VkPresentInfoKHR& presentInfo) {
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    switch (Result result = vkQueuePresentKHR(presentationQueue, &presentInfo)) {
        case VK_SUCCESS:
            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            if (Result result = recreateSwapchain(); result != VK_SUCCESS) {
                std::cerr << "Failed to recreate swapchain after it became out of date or suboptimal during presentation: " << string_VkResult(result) << std::endl;
                return result;
            }
            break;
        default:
            std::cerr << "Failed to present image: " << string_VkResult(result) << std::endl;
            return result;
    }

    return VK_SUCCESS;
}

Result VulkanBase::presentImage(VkSemaphore renderFinishedSemaphore) {
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &currentImageIndex,
    };

    if (renderFinishedSemaphore) {
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
    }

    return presentImage(presentInfo);
}
