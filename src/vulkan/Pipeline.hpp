#pragma once

#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>


class Pipeline {
private:
    VkPipeline handle{};

public:
    Pipeline();
    Pipeline(const VkGraphicsPipelineCreateInfo& createInfo);
    Pipeline(const VkComputePipelineCreateInfo& createInfo);
    Pipeline(Pipeline&& other) noexcept;
    ~Pipeline();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    Result create(VkGraphicsPipelineCreateInfo& createInfo);
    Result create(VkComputePipelineCreateInfo& createInfo);
};

#endif // _PIPELINE_HPP_