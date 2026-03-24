#include "GLFW/glfw3.h"
#include "vulkan/CommandBuffer.hpp"
#include "vulkan/CommandPool.hpp"
#include "vulkan/Fence.hpp"
#include "vulkan/Semaphore.hpp"
#include "vulkan/VulkanBase.hpp"
#include "window/Window.hpp"

int main() {
    Window window(800, 600, "Polynomial", true);
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();

    window.show();
}