#pragma once

#ifndef _GRAPHICS_PIPELINE_CREATE_INFO_PACK_HPP_
#define _GRAPHICS_PIPELINE_CREATE_INFO_PACK_HPP_

#include <vector>
#include <vulkan/vulkan.h>

struct GraphicsPipelineCreateInfoPack {
    VkGraphicsPipelineCreateInfo createInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;

    uint32_t dynamicViewportCount = 1;
    uint32_t dynamicScissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    std::vector<VkDynamicState> dynamicStates;

    GraphicsPipelineCreateInfoPack();
    GraphicsPipelineCreateInfoPack(const GraphicsPipelineCreateInfoPack& other) noexcept;

    operator VkGraphicsPipelineCreateInfo&();

    void updateAllArrays();

private:
    void setCreateInfos();
    void updateAllArrayAddresses();
};

#endif // _GRAPHICS_PIPELINE_CREATE_INFO_PACK_HPP_