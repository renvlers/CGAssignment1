#pragma once

#ifndef _PIPELINE_LAYOUT_HPP_
#define _PIPELINE_LAYOUT_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class PipelineLayout {
private:
    VkPipelineLayout handle{};

public:
    PipelineLayout();
    PipelineLayout(const VkPipelineLayoutCreateInfo& createInfo);
    PipelineLayout(PipelineLayout&& other) noexcept;
    ~PipelineLayout();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result create(VkPipelineLayoutCreateInfo& createInfo);
};

#endif // _PIPELINE_LAYOUT_HPP_