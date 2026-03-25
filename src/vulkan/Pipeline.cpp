#include "Pipeline.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

Pipeline::Pipeline() = default;
Pipeline::Pipeline(const VkGraphicsPipelineCreateInfo& createInfo) {
    VkGraphicsPipelineCreateInfo info = createInfo;
    create(info);
}
Pipeline::Pipeline(const VkComputePipelineCreateInfo& createInfo) {
    VkComputePipelineCreateInfo info = createInfo;
    create(info);
}
Pipeline::Pipeline(Pipeline&& other) noexcept { MOVE_HANDLE; }
Pipeline::~Pipeline() { DESTROY_HANDLE_BY(vkDestroyPipeline); }

Result Pipeline::create(VkGraphicsPipelineCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkCreateGraphicsPipelines(vulkanBase.getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create graphics pipeline: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result Pipeline::create(VkComputePipelineCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkCreateComputePipelines(vulkanBase.getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create compute pipeline: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}