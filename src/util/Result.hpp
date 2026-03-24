#pragma once

#ifndef _RESULT_HPP_
#define _RESULT_HPP_

#include <vulkan/vulkan.h>

#ifdef VK_RESULT_THROW

class Result {
private:
    VkResult result;

public:
    static inline void (*callbackThrow)(VkResult);
    Result(VkResult result);
    Result(Result&& other) noexcept;
    ~Result() noexcept(false);

    explicit operator VkResult() const;
};

#elifdef VK_RESULT_NODISCARD

struct [[nodiscard]] Result {
private:
    VkResult result;

public:
    Result(VkResult result);

    explicit operator VkResult() const;
};

#pragma warning(disable : 4834) // Suppress warning about discarding the result of a function that returns a value marked with [[nodiscard]]
#pragma warning(disable : 6031) // Suppress warning about ignoring the return value of a function that may fail

#else
using Result = VkResult;

#endif // VK_RESULT_THROW

#endif // _RESULT_HPP_