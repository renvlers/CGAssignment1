#pragma once

#include <cstdint>
#ifndef _VULKAN_BASE_HPP_
#define _VULKAN_BASE_HPP_

#include <span>
#include <vector>
#include <vulkan/vulkan.h>

/**
 * @class VulkanBase
 * @brief A base class for Vulkan applications, providing common functionality for setting up Vulkan.
 *
 * @note This class uses singleton pattern to ensure that only one instance of VulkanBase exists throughout the application.
 */
class VulkanBase {
private:
    /**
     * @name Vulkan API version
     *
     */
    uint32_t apiVersion = VK_API_VERSION_1_0;

    /**
     * @name Vulkan instance, instance layers, and instance extensions
     */
    /// @{
    VkInstance instance;
    std::vector<const char*> instanceLayers;
    std::vector<const char*> instanceExtensions;
    /// @}

    /**
     * @name Vulkan physical device, physical device properties, physical device memory properties and available physical devices
     */
    /// @{
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    std::vector<VkPhysicalDevice> availablePhysicalDevices;
    /// @}

    /**
     * @name Vulkan logical device and device extensions
     */
    /// @{
    VkDevice device;
    std::vector<const char*> deviceExtensions;
    /// @}

    /**
     * @name Vulkan queues and their respective family indices
     */
    /// @{
    VkQueue graphicsQueue;
    VkQueue presentationQueue;
    VkQueue computeQueue;
    uint32_t queueFamilyIndexGraphics = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndexPresentation = VK_QUEUE_FAMILY_IGNORED;
    uint32_t queueFamilyIndexCompute = VK_QUEUE_FAMILY_IGNORED;
    /// @}

    /**
     * @name Vulkan surface
     */
    VkSurfaceKHR surface;

    /**
     * @name Vulkan debug messenger
     */
    VkDebugUtilsMessengerEXT debugMessenger;

    /**
     * @name Vulkan surface formats
     *
     */
    std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;

    /**
     * @name Vulkan swapchain
     *
     */
    VkSwapchainKHR swapchain;

    /**
     * @name Vulkan swapchain images and image views
     *
     */
    /// @{
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    /// @}

    /**
     * @name Vulkan swapchain create info
     *
     */
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

    /**
     * @brief Private constructors, assignment operators and destructor to prevent multiple instances of VulkanBase.
     */
    /// @{
    VulkanBase() = default;
    VulkanBase(const VulkanBase&) = delete;
    VulkanBase(VulkanBase&&) = delete;
    VulkanBase& operator=(const VulkanBase&) = delete;
    VulkanBase& operator=(VulkanBase&&) = delete;
    ~VulkanBase();
    /// @}

    /**
     * @brief Add a layer or extension to the specified container.
     */
    static inline void addLayerOrExtension(std::vector<const char*>& container, const char* name);

    /**
     * @brief Create Vulkan debug messenger.
     */
    VkResult createDebugMessenger();

    /**
     * @brief Private method to create swapchain.
     *
     */
    VkResult createSwapchainInternal();

public:
    /**
     * @brief Get the singleton instance of VulkanBase.
     *
     * @return Reference to the singleton instance of VulkanBase.
     */
    static VulkanBase& getVulkanBase();

    /**
     * @brief Get the API version being used by VulkanBase.
     *
     * @return Vulkan API version.
     */
    uint32_t getApiVersion() const;

    /**
     * @brief Set the Vulkan API to latest version supported by the system.
     *
     * @return VK_SUCCESS if the API version was set successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult setApiVersionToLatest();

    /**
     * @brief Create Vulkan instance with the specified flags.
     *
     * @param flags Optional flags for instance creation.
     * @return VK_SUCCESS if the instance was created successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult createInstance(VkInstanceCreateFlags flags = 0);

    /**
     * @brief Get the Vulkan instance.
     *
     * @return Vulkan instance handle.
     */
    VkInstance getInstance() const;

    /**
     * @brief Add an instance layer to the Vulkan instance.
     *
     * @param layerName Name of the layer to add.
     */
    void addInstanceLayer(const char* layerName);

    /**
     * @brief Add an instance extension to the Vulkan instance.
     *
     * @param extensionName Name of the extension to add.
     */
    void addInstanceExtension(const char* extensionName);

    /**
     * @brief Add a device extension to the Vulkan device.
     *
     * @param extensionName Name of the extension to add.
     */
    void addDeviceExtension(const char* extensionName);

    /**
     * @brief Set the instance layers for the Vulkan instance.
     *
     * @param layers Span of layer names to set.
     */
    void setInstanceLayers(std::span<const char*> layers);

    /**
     * @brief Set the instance extensions for the Vulkan instance.
     *
     * @param extensions Span of extension names to set.
     */
    void setInstanceExtensions(std::span<const char*> extensions);

    /**
     * @brief Set the device extensions for the Vulkan device.
     *
     * @param extensions Span of extension names to set.
     */
    void setDeviceExtensions(std::span<const char*> extensions);

    /**
     * @brief Get the instance layers enabled for the Vulkan instance.
     *
     * @return Reference to the vector of instance layer names.
     */
    const std::vector<const char*>& getInstanceLayers() const;

    /**
     * @brief Get the instance extensions enabled for the Vulkan instance.
     *
     * @return Reference to the vector of instance extension names.
     */
    const std::vector<const char*>& getInstanceExtensions() const;

    /**
     * @brief Get the device extensions enabled for the Vulkan device.
     *
     * @return Reference to the vector of device extension names.
     */
    const std::vector<const char*>& getDeviceExtensions() const;

    /**
     * @brief Check if the specified instance layers are available.
     *
     * @param layersToCheck Span of layer names to check for availability.
     * @return VK_SUCCESS if all layers are available, otherwise returns an appropriate VkResult error code.
     */
    VkResult checkInstanceLayers(std::span<const char*> layersToCheck) const;

    /**
     * @brief Check if the specified instance extensions are available.
     *
     * @param extensionsToCheck Span of extension names to check for availability.
     * @param layerName Optional name of the layer to check for extensions, if nullptr checks for global extensions.
     * @return VK_SUCCESS if all extensions are available, otherwise returns an appropriate VkResult error code.
     */
    VkResult checkInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const;

    /**
     * @brief Check if the specified device extensions are available.
     *
     * @param extensionsToCheck Span of extension names to check for availability.
     * @return VK_SUCCESS if all extensions are available, otherwise returns an appropriate VkResult error code.
     */
    VkResult checkDeviceExtensions(std::span<const char*> extensionsToCheck) const;

    /**
     * @brief Get the current physical device.
     *
     * @return Physical device handle.
     */
    VkPhysicalDevice getCurrentPhysicalDevice() const;

    /**
     * @brief Get physical device properties.
     *
     * @return Reference to the physical device properties structure.
     */
    const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const;

    /**
     * @brief Get physical device memory properties.
     *
     * @return Reference to the physical device memory properties structure.
     */
    const VkPhysicalDeviceMemoryProperties& getPhysicalDeviceMemoryProperties() const;

    /**
     * @brief Get an available physical device by index.
     *
     * @param index Index of the physical device to retrieve.
     * @return Physical device handle at the specified index.
     */
    VkPhysicalDevice getAvailablePhysicalDevice(uint32_t index) const;

    /**
     * @brief Get all available physical devices.
     *
     * @return Reference to the vector of available physical device handles.
     */
    const std::vector<VkPhysicalDevice>& getAvailablePhysicalDevices() const;

    /**
     * @brief Find all available physical devices
     *
     * @return VK_SUCCESS if physical devices were found successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult findAvailablePhysicalDevices();

    /**
     * @brief Determine physical device to use based on the specified index and required queue support.
     *
     * @param deviceIndex Index of the physical device to check for queue family support.
     * @param enableGraphicsQueue Whether to require support for graphics operations.
     * @param enableComputeQueue Whether to require support for compute operations.
     *
     * @return VK_SUCCESS if the queue family indices were found successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult determinePhysicalDevice(uint32_t deviceIndex = 0, bool enableGraphicsQueue = true, bool enableComputeQueue = true);

    /**
     * @brief Get the count of available physical devices.
     *
     * @return Count of available physical devices.
     */
    uint32_t getAvailablePhysicalDeviceCount() const;

    /**
     * @brief Create a Vulkan device.
     *
     * @param flags Optional flags for device creation, default is 0.
     *
     * @return VK_SUCCESS if the device was created successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult createDevice(VkDeviceCreateFlags flags = 0);

    /**
     * @brief Get the logical device.
     *
     * @return Logical device handle.
     */
    VkDevice getDevice() const;

    /**
     * @brief Get the queue family index for graphics, presentation, or compute operations.
     *
     * @param physicalDevice Physical device to check for queue family support.
     * @param enableGraphics Whether to require support for graphics operations.
     * @param enableCompute Whether to require support for compute operations.
     * @param queueFamilyIndices Output array to store the queue family indices for graphics, presentation, and compute operations.
     *
     * @return VK_SUCCESS if the queue family indices were found successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult getQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphics, bool enableCompute, uint32_t (&queueFamilyIndices)[3]);

    /**
     * @brief Get the queue family index.
     *
     * @return Queue family index, or VK_QUEUE_FAMILY_IGNORED if not found.
     */
    /// @{
    uint32_t getQueueFamilyIndexGraphics() const;
    uint32_t getQueueFamilyIndexPresentation() const;
    uint32_t getQueuefamilyIndexCompute() const;
    /// @}

    /**
     * @brief Get the Vulkan queues.
     *
     */
    /// @{
    VkQueue getGraphicsQueue() const;
    VkQueue getPresentationQueue() const;
    VkQueue getComputeQueue() const;
    /// @}

    /**
     * @brief Get the Vulkan surface.
     *
     * @return Vulkan surface handle.
     */
    VkSurfaceKHR getSurface() const;

    /**
     * @brief Initialize the Vulkan surface.
     *
     * @param surface Vulkan surface handle to initialize with.
     */
    void initializeSurface(VkSurfaceKHR surface);

    /**
     * @brief Get available surface format by index.
     *
     * @param index Index of the surface format to retrieve.
     *
     * @return Surface format at the specified index.
     */
    const VkSurfaceFormatKHR& getAvailableSurfaceFormat(uint32_t index) const;

    /**
     * @brief Get available surface color space by index.
     *
     * @param index Index of the color space to retrieve.
     *
     * @return Color space at the specified index.
     */
    const VkColorSpaceKHR& getAvailableSurfaceColorSpace(uint32_t index) const;

    /**
     * @brief Get the count of available surface formats.
     *
     * @return Count of available surface formats.
     */
    uint32_t getAvailableSurfaceFormatCount() const;

    /**
     * @brief Get the swapchain handle.
     *
     */
    VkSwapchainKHR getSwapchain() const;

    /**
     * @brief Get the swapchain images by index.
     *
     * @param index Index of the swapchain image to retrieve.
     *
     * @return Swapchain image at the specified index.
     */
    VkImage getSwapchainImage(uint32_t index) const;

    /**
     * @brief Get the swapchain image views by index.
     *
     * @param index Index of the swapchain image view to retrieve.
     *
     * @return Swapchain image view at the specified index.
     */
    VkImageView getSwapchainImageView(uint32_t index) const;

    /**
     * @brief Get the count of swapchain images.
     *
     * @return Count of swapchain images.
     */
    uint32_t getSwapchainImageCount() const;

    /**
     * @brief Get the swapchain create info.
     *
     * @return Reference to the swapchain create info structure.
     */
    const VkSwapchainCreateInfoKHR& getSwapchainCreateInfo() const;

    /**
     * @brief Get surface formats.
     *
     * @return Reference to the vector of available surface formats.
     */
    const std::vector<VkSurfaceFormatKHR>& getAvailableSurfaceFormats() const;

    /**
     * @brief Set surface format.
     *
     * @param surfaceFormat Surface format to set for the swapchain.
     *
     * @return VK_SUCCESS if the surface format was set successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult setSurfaceFormat(const VkSurfaceFormatKHR& surfaceFormat);

    /**
     * @brief Create Vulkan swapchain.
     *
     * @param limitFrameRate Whether to limit the frame rate to the display's refresh rate.
     * @param flags Optional flags for swapchain creation, default is 0.
     *
     * @return VK_SUCCESS if the swapchain was created successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult createSwapchain(bool limitFrameRate = true, VkSwapchainCreateFlagsKHR flags = 0);

    /**
     * @brief Recreate the swapchain, typically called when the window is resized or the surface becomes incompatible.
     *
     * @return VK_SUCCESS if the swapchain was recreated successfully, otherwise returns an appropriate VkResult error code.
     */
    VkResult recreateSwapchain();
};

#endif // _VULKAN_BASE_HPP_