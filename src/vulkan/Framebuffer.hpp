#pragma once

#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class Framebuffer {
private:
    VkFramebuffer handle;

public:
    Framebuffer();
    Framebuffer(const VkFramebufferCreateInfo& createInfo);
    Framebuffer(Framebuffer&& other) noexcept;
    ~Framebuffer();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result create(VkFramebufferCreateInfo& createInfo);
};

#endif // _FRAMEBUFFER_HPP_