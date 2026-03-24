#pragma once

#ifndef _SEMAPHORE_HPP_
#define _SEMAPHORE_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class Semaphore {
private:
    VkSemaphore handle = VK_NULL_HANDLE;

public:
    Semaphore(const VkSemaphoreCreateInfo& createInfo);
    Semaphore(VkSemaphoreCreateFlags flags = 0);
    Semaphore(Semaphore&& other) noexcept;
    ~Semaphore();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result create(VkSemaphoreCreateInfo& createInfo);
    Result create(VkSemaphoreCreateFlags flags = 0);
};

#endif // _SEMAPHORE_HPP_