#include "Semaphore.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

Semaphore::Semaphore(const VkSemaphoreCreateInfo& createInfo) {
    VkSemaphoreCreateInfo info = createInfo;
    create(info);
}
Semaphore::Semaphore(VkSemaphoreCreateFlags flags) { create(flags); }
Semaphore::Semaphore(Semaphore&& other) noexcept { MOVE_HANDLE; }
Semaphore::~Semaphore() { DESTROY_HANDLE_BY(vkDestroySemaphore); }

Result Semaphore::create(VkSemaphoreCreateInfo& createInfo) {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (Result result = vkCreateSemaphore(vulkanBase.getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create a semaphore: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result Semaphore::create(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.flags = flags;

    return create(createInfo);
}