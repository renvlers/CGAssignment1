#include "Window.hpp"
#include "../vulkan/VulkanBase.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

Window::Window(uint32_t width, uint32_t height, const char* title, bool resizable) : width(width), height(height), title(title), resizable(resizable), window(nullptr) {}
Window::~Window() {
    if (window) {
        destroy();
    }
}

void Window::show(std::function<void()> renderCallback) {
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
    if (VkResult result = glfwCreateWindowSurface(vulkanBase.getInstance(), window, nullptr, &surface); result != VK_SUCCESS) {
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

    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();

        // Rendering logic will be handled by the provided callback function.
        if (renderCallback) {
            renderCallback();
        }
    }
}

bool Window::shouldClose() const { return glfwWindowShouldClose(this->window); }

void Window::destroy() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
    }
}