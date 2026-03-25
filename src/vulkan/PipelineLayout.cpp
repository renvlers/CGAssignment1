#include "PipelineLayout.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

PipelineLayout::PipelineLayout() = default;
PipelineLayout::PipelineLayout(const VkPipelineLayoutCreateInfo& createInfo) {
    VkPipelineLayoutCreateInfo info = createInfo;
    create(info);
}
PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept { MOVE_HANDLE; }
PipelineLayout::~PipelineLayout() { DESTROY_HANDLE_BY(vkDestroyPipelineLayout); }

Result PipelineLayout::create(VkPipelineLayoutCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkCreatePipelineLayout(vulkanBase.getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create pipeline layout: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}