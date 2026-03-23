#include "VulkanBase.hpp"
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

VulkanBase::~VulkanBase() {
    // TODO: Clean up Vulkan resources (instance, device, debug messenger, etc.)
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

VkResult VulkanBase::createDebugMessenger() {
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
        VkResult result = vkCreateDebugUtilsMessenger(this->instance, &debugUtilsMessengerCreateInfo, nullptr, &this->debugMessenger);
        if (result != VK_SUCCESS) {
            std::cerr << "Failed to create Vulkan debug messenger: " << string_VkResult(result) << std::endl;
        }
        return result;
    }
    std::cerr << "Failed to get function pointer for vkCreateDebugUtilsMessengerEXT, debug messenger will not be created." << std::endl;
    return VK_RESULT_MAX_ENUM; // Return an error code to indicate that the debug messenger was not created
}

VkResult VulkanBase::createSwapchainInternal() {
    // TODO: Implement internal swapchain creation logic, which may involve querying surface capabilities, selecting surface format and present mode, and creating the swapchain with vkCreateSwapchainKHR
    return VK_SUCCESS;
}

VulkanBase& VulkanBase::getVulkanBase() {
    static VulkanBase instance;
    return instance;
}

uint32_t VulkanBase::getApiVersion() const { return this->apiVersion; }
VkResult VulkanBase::setApiVersionToLatest() {
    if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")) {
        return vkEnumerateInstanceVersion(&apiVersion);
    }
    return VK_SUCCESS;
}

VkResult VulkanBase::createInstance(VkInstanceCreateFlags flags) {
#ifndef NDEBUG
    addInstanceLayer("VK_LAYER_KHRONOS_validation");
    addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
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

    if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance: " << string_VkResult(result) << std::endl;
        return result;
    }

    std::cout << std::format("Vulkan instance created with API version {}.{}.{}", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion)) << std::endl;

#ifndef NDEBUG
    createDebugMessenger();
#endif

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

VkResult VulkanBase::checkInstanceLayers(std::span<const char*> layersToCheck) const {
    uint32_t layerCount = 0;
    std::vector<VkLayerProperties> availableLayers;
    if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of instance layers: " << string_VkResult(result) << std::endl;
        return result;
    }

    if (layerCount > 0) {
        availableLayers.resize(layerCount);
        if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); result != VK_SUCCESS) {
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

VkResult VulkanBase::checkInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName) const {
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> availableExtensions;
    if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of instance extensions for layer " << (layerName ? layerName : "global") << ": " << string_VkResult(result) << std::endl;
        return result;
    }

    if (extensionCount > 0) {
        availableExtensions.resize(extensionCount);
        if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, availableExtensions.data()); result != VK_SUCCESS) {
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

VkResult VulkanBase::checkDeviceExtensions(std::span<const char*> extensionsToCheck) const {
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

VkResult VulkanBase::findAvailablePhysicalDevices() {
    uint32_t deviceCount = 0;
    if (VkResult result = vkEnumeratePhysicalDevices(this->instance, &deviceCount, nullptr); result != VK_SUCCESS) {
        std::cerr << "Failed to get the count of physical devices: " << string_VkResult(result);
        return result;
    }

    if (!deviceCount) {
        std::cerr << "No physical devices found that support Vulkan." << std::endl;
        abort(); // No devices found, cannot continue
    }

    availablePhysicalDevices.resize(deviceCount);
    if (VkResult result = vkEnumeratePhysicalDevices(this->instance, &deviceCount, availablePhysicalDevices.data()); result != VK_SUCCESS) {
        std::cerr << "Failed to enumerate physical devices: " << string_VkResult(result);
        return result;
    }

    return VK_SUCCESS;
}

VkResult VulkanBase::determinePhysicalDevice(uint32_t deviceIndex, bool enableGraphicsQueue, bool enableComputeQueue) {
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
        VkResult result = getQueueFamilyIndices(availablePhysicalDevices[deviceIndex], enableGraphicsQueue, enableComputeQueue, indices);

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

VkResult VulkanBase::createDevice(VkDeviceCreateFlags flags) {
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

    if (VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device); result != VK_SUCCESS) {
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

    return VK_SUCCESS;
}

VkDevice VulkanBase::getDevice() const { return this->device; }

VkResult VulkanBase::getQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphics, bool enableCompute, uint32_t (&queueFamilyIndices)[3]) {
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
            if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportPresentation); result != VK_SUCCESS) {
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

VkResult VulkanBase::setSurfaceFormat(const VkSurfaceFormatKHR& surfaceFormat) {
    // TODO: Implement surface format selection and swapchain recreation if necessary based on the provided surface format
    return VK_SUCCESS;
}

VkResult VulkanBase::createSwapchain(bool limitFrameRate, VkSwapchainCreateFlagsKHR flags) {
    // TODO: Implement swapchain creation using vkCreateSwapchainKHR with the current surface, selected surface format, and
    // specified flags, optionally limiting frame rate to the display's refresh rate
    return VK_SUCCESS;
}

VkResult VulkanBase::recreateSwapchain() {
    // TODO: Implement swapchain recreation, which involves destroying the existing swapchain and creating a new one with the
    // current surface and selected surface format
    return VK_SUCCESS;
}