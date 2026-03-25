#include "Window.hpp"
#include "../polynomial/Polynomial.hpp"
#include "../polynomial/PolynomialDrawer.hpp"
#include "../vulkan/CommandBuffer.hpp"
#include "../vulkan/CommandPool.hpp"
#include "../vulkan/Fence.hpp"
#include "../vulkan/Functions.hpp"
#include "../vulkan/Semaphore.hpp"
#include "../vulkan/VulkanBase.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>

Window::Window(uint32_t width, uint32_t height, const char* title, bool resizable, bool limitFrameRate) : width(width), height(height), title(title), resizable(resizable), limitFrameRate(limitFrameRate), window(nullptr) {}
Window::~Window() {
    if (window) {
        destroy();
    }
}

void Window::show() {
#ifndef NDEBUG
    glfwSetErrorCallback([](int error, const char* description) { std::cerr << ">>> [GLFW ERROR] " << error << ": " << description << std::endl; });
#endif

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, resizable);

    this->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!this->window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        destroy();
        return;
    }

    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();

    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (!extensions) {
        std::cerr << "Failed to get required instance extensions from GLFW" << std::endl;
        destroy();
        return;
    }

    for (size_t i = 0; i < extensionCount; i++) {
        vulkanBase.addInstanceExtension(extensions[i]);
    }

    vulkanBase.setApiVersionToLatest();
    if (vulkanBase.createInstance() != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance" << std::endl;
        destroy();
        return;
    }

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (Result result = glfwCreateWindowSurface(vulkanBase.getInstance(), window, nullptr, &surface); result != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan surface: " << string_VkResult(result) << std::endl;
        destroy();
        return;
    }
    vulkanBase.initializeSurface(surface);

    vulkanBase.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    if (vulkanBase.findAvailablePhysicalDevices() != VK_SUCCESS || vulkanBase.determinePhysicalDevice() != VK_SUCCESS || vulkanBase.createDevice() != VK_SUCCESS) {
        std::cerr << "Failed to setup Vulkan physical/logical device" << std::endl;
        destroy();
        return;
    }

    if (vulkanBase.createSwapchain(limitFrameRate) != VK_SUCCESS) {
        std::cerr << "Failed to create swapchain" << std::endl;
        destroy();
        return;
    }

    const auto& [renderPass, framebuffers] = getRpwf();
    createLayout();
    createPipeline();

    Fence fence(VK_FENCE_CREATE_SIGNALED_BIT);
    Semaphore imageAvailableSemaphore;
    Semaphore renderFinishedSemaphore;

    CommandBuffer commandBuffer;
    CommandPool commandPool(vulkanBase.getQueueFamilyIndexGraphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    commandPool.allocateCommandBuffers(commandBuffer);

    VkClearValue clearColor = {
        .color = {{1.0f, 1.0f, 1.0f, 1.0f}},
    };

    Polynomial polynomial({2, -3, 1}); // Example polynomial: f(x) = 2x^2 - 3x + 1
    PolynomialDrawer drawer(std::move(polynomial), -10, 10);

    std::vector<Vertex> vertices = drawer.draw();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer vertexBuffer;
    vkCreateBuffer(vulkanBase.getDevice(), &bufferInfo, nullptr, &vertexBuffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanBase.getDevice(), vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanBase.getCurrentPhysicalDevice(), &memProperties);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory vertexBufferMemory;
    vkAllocateMemory(vulkanBase.getDevice(), &allocInfo, nullptr, &vertexBufferMemory);
    vkBindBufferMemory(vulkanBase.getDevice(), vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(vulkanBase.getDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), bufferInfo.size);
    vkUnmapMemory(vulkanBase.getDevice(), vertexBufferMemory);

    VkBuffer vertexBuffers[]{vertexBuffer};
    VkDeviceSize offsets[]{0};

    while (!glfwWindowShouldClose(this->window)) {
        while (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            glfwWaitEvents();
        }

        vulkanBase.swapImage(imageAvailableSemaphore);
        auto i = vulkanBase.getCurrentImageIndex();

        commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        renderPass.cmdBegin(commandBuffer, framebuffers[i], {{}, vulkanBase.getSwapchainCreateInfo().imageExtent}, clearColor);

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);

        renderPass.cmdEnd(commandBuffer);
        commandBuffer.end();

        vulkanBase.submitCommandBufferToGraphicsQueue(commandBuffer, imageAvailableSemaphore, renderFinishedSemaphore, fence);
        vulkanBase.presentImage(renderFinishedSemaphore);

        fence.waitAndReset();

        glfwPollEvents();
    }
}

bool Window::shouldClose() const { return glfwWindowShouldClose(this->window); }

void Window::destroy() {
    if (window) {
        VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
        vulkanBase.waitForDeviceIdle();
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
    }
}