#include "CommandBuffer.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

CommandBuffer::CommandBuffer() = default;
CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept { MOVE_HANDLE; }

Result CommandBuffer::begin(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo) const {
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags,
        .pInheritanceInfo = &inheritanceInfo,
    };

    if (Result result = vkBeginCommandBuffer(handle, &beginInfo); result != VK_SUCCESS) {
        std::cerr << "Failed to begin recording command buffer: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result CommandBuffer::begin(VkCommandBufferUsageFlags usageFlags) const {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usageFlags,
    };

    if (Result result = vkBeginCommandBuffer(handle, &beginInfo); result != VK_SUCCESS) {
        std::cerr << "Failed to begin recording command buffer: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result CommandBuffer::end() const {
    if (Result result = vkEndCommandBuffer(handle); result != VK_SUCCESS) {
        std::cerr << "Failed to end recording command buffer: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}