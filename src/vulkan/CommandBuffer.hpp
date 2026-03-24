#pragma once

#ifndef _COMMAND_BUFFER_HPP_
#define _COMMAND_BUFFER_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class CommandBuffer {
private:
    friend class CommandPool;
    VkCommandBuffer handle;

public:
    CommandBuffer();
    CommandBuffer(CommandBuffer&& other) noexcept;

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result begin(VkCommandBufferUsageFlags usageFlags, VkCommandBufferInheritanceInfo& inheritanceInfo) const;
    Result begin(VkCommandBufferUsageFlags usageFlags = 0) const;

    Result end() const;
};

#endif // _COMMAND_BUFFER_HPP_