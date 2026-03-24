#pragma once

#ifndef _DEFINES_HPP_
#define _DEFINES_HPP_

#ifndef NDEBUG
constexpr bool ENABLE_DEBUG_MESSENGER = true;
#else
constexpr bool ENABLE_DEBUG_MESSENGER = false;
#endif // NDEBUG

#define DESTROY_HANDLE_BY(func)                                                                                                                                                                                                                                \
    if (handle) {                                                                                                                                                                                                                                              \
        func(VulkanBase::getVulkanBase().getDevice(), handle, nullptr);                                                                                                                                                                                        \
        handle = VK_NULL_HANDLE;                                                                                                                                                                                                                               \
    }

#define MOVE_HANDLE                                                                                                                                                                                                                                            \
    handle = other.handle;                                                                                                                                                                                                                                     \
    other.handle = VK_NULL_HANDLE

#define DEFINE_HANDLE_TYPE_OPERATOR                                                                                                                                                                                                                            \
    operator decltype(handle)() const { return handle; }

#define DEFINE_ADDRESS_FUNCTION                                                                                                                                                                                                                                \
    const decltype(handle)* address() const { return &handle; }

#define EXECUTE_ONCE(...)                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                          \
        static bool executed = false;                                                                                                                                                                                                                          \
        if (executed) {                                                                                                                                                                                                                                        \
            return __VA_ARGS__;                                                                                                                                                                                                                                \
        }                                                                                                                                                                                                                                                      \
        executed = true;                                                                                                                                                                                                                                       \
    }

#endif // _DEFINES_HPP_