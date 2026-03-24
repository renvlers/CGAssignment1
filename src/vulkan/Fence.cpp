#include "Fence.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

Fence::Fence(const VkFenceCreateInfo& createInfo) {
    VkFenceCreateInfo info = createInfo;
    create(info);
}
Fence::Fence(VkFenceCreateFlags flags) { create(flags); }
Fence::Fence(Fence&& other) noexcept { MOVE_HANDLE; }
Fence::~Fence() { DESTROY_HANDLE_BY(vkDestroyFence); }

Result Fence::wait() const {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkWaitForFences(vulkanBase.getDevice(), 1, &handle, VK_FALSE, UINT64_MAX); result != VK_SUCCESS) {
        std::cerr << "Failed to wait for fence: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result Fence::reset() const {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkResetFences(vulkanBase.getDevice(), 1, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to reset fence: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result Fence::waitAndReset() const {
    Result result = wait();
    result || (result = reset());
    return result;
}

Result Fence::status() const {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    Result result = vkGetFenceStatus(vulkanBase.getDevice(), handle);

    if (result < VK_SUCCESS) {
        std::cerr << "Failed to get the status of the fence: " << string_VkResult(result) << std::endl;
    }

    return result;
}

Result Fence::create(VkFenceCreateInfo& createInfo) {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (Result result = vkCreateFence(vulkanBase.getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create a fence: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result Fence::create(VkFenceCreateFlags flags) {
    VkFenceCreateInfo createInfo{.flags = flags};
    return create(createInfo);
}
