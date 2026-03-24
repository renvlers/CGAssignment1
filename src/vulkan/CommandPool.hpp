#pragma once

#ifndef _COMMAND_POOL_HPP_
#define _COMMAND_POOL_HPP_

#include "../util/ArrayRef.hpp"
#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include "CommandBuffer.hpp"
#include <vulkan/vulkan.h>

class CommandPool {
private:
    VkCommandPool handle{VK_NULL_HANDLE};

public:
    CommandPool();
    CommandPool(const VkCommandPoolCreateInfo& createInfo);
    CommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    ~CommandPool();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result allocateCommandBuffers(ArrayRef<VkCommandBuffer> buffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;
    Result allocateCommandBuffers(ArrayRef<CommandBuffer> buffers, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

    void freeCommandBuffers(ArrayRef<VkCommandBuffer> buffers) const;
    void freeCommandBuffers(ArrayRef<CommandBuffer> buffers) const;

    void trim() const;

    Result create(VkCommandPoolCreateInfo& createInfo);
    Result create(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
};

#endif // _COMMAND_POOL_HPP_