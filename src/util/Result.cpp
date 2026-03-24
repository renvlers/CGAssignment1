#ifdef VK_RESULT_THROW

#include "Result.hpp"

Result::Result(VkResult result) : result(result) {}

Result::Result(Result&& other) noexcept : result(other.result) {
    other.result = VK_SUCCESS; // Reset the result in the moved-from object to a success code to prevent potential issues if it's used after being moved from
}

Result::~Result() noexcept(false) {
    if (uint32_t(result) < VK_RESULT_MAX_ENUM) {
        return;
    }
    if (callbackThrow) {
        callbackThrow(result);
    }
    throw result;
}

explicit operator VkResult() const {
    VkResult result = this->result;
    this->result = VK_SUCCESS; // Reset the result to a success code after it's been retrieved to prevent potential issues if it's used again after being retrieved
    return result;
}

#elifdef VK_RESULT_NODISCARD

#include "Result.hpp"

Result::Result(VkResult result) : result(result) {}

explicit operator VkResult() const { return result; }

#endif // VK_RESULT_THROW, VK_RESULT_NODISCARD