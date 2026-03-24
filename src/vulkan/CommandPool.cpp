#include "CommandPool.hpp"
#include "VulkanBase.hpp"
#include <cstdint>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

CommandPool::CommandPool() = default;
CommandPool::CommandPool(const VkCommandPoolCreateInfo& createInfo) {
    VkCommandPoolCreateInfo info = createInfo;
    create(info);
}
CommandPool::CommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) { create(queueFamilyIndex, flags); }
CommandPool::~CommandPool() { DESTROY_HANDLE_BY(vkDestroyCommandPool); }

Result CommandPool::allocateCommandBuffers(ArrayRef<VkCommandBuffer> buffers, VkCommandBufferLevel level) const {
    VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = handle,
        .level = level,
        .commandBufferCount = static_cast<uint32_t>(buffers.size()),
    };

    if (Result result = vkAllocateCommandBuffers(VulkanBase::getVulkanBase().getDevice(), &allocateInfo, buffers.pointer()); result != VK_SUCCESS) {
        std::cerr << "Failed to allocate command buffers: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}
Result CommandPool::allocateCommandBuffers(ArrayRef<CommandBuffer> buffers, VkCommandBufferLevel level) const { return allocateCommandBuffers({&buffers[0].handle, buffers.size()}, level); }

void CommandPool::freeCommandBuffers(ArrayRef<VkCommandBuffer> buffers) const {
    vkFreeCommandBuffers(VulkanBase::getVulkanBase().getDevice(), handle, static_cast<uint32_t>(buffers.size()), buffers.pointer());
    memset(buffers.pointer(), 0, buffers.size() * sizeof(VkCommandBuffer));
}
void CommandPool::freeCommandBuffers(ArrayRef<CommandBuffer> buffers) const { freeCommandBuffers({&buffers[0].handle, buffers.size()}); }

void CommandPool::trim() const {
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    vkTrimCommandPool(vulkanBase.getDevice(), handle, 0);
}

Result CommandPool::create(VkCommandPoolCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    if (Result result = vkCreateCommandPool(VulkanBase::getVulkanBase().getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create command pool: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result CommandPool::create(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo createInfo{
        .flags = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };

    return create(createInfo);
}