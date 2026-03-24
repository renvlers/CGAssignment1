#pragma once

#ifndef _FENCE_HPP_
#define _FENCE_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class Fence {
private:
    VkFence handle = VK_NULL_HANDLE;

public:
    Fence(const VkFenceCreateInfo& createInfo);
    Fence(VkFenceCreateFlags flags = 0);
    Fence(Fence&& other) noexcept;
    ~Fence();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result wait() const;
    Result reset() const;
    Result waitAndReset() const;

    Result status() const;

    Result create(VkFenceCreateInfo& createInfo);
    Result create(VkFenceCreateFlags flags = 0);
};

#endif // _FENCE_HPP_