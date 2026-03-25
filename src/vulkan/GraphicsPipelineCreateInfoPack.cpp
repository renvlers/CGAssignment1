#include "GraphicsPipelineCreateInfoPack.hpp"

GraphicsPipelineCreateInfoPack::GraphicsPipelineCreateInfoPack() {
    setCreateInfos();
    createInfo.basePipelineIndex = -1;
}

GraphicsPipelineCreateInfoPack::GraphicsPipelineCreateInfoPack(const GraphicsPipelineCreateInfoPack& other) noexcept
    : createInfo(other.createInfo), shaderStages(other.shaderStages), vertexInputStateCreateInfo(other.vertexInputStateCreateInfo), vertexInputBindingDescriptions(other.vertexInputBindingDescriptions),
      vertexInputAttributeDescriptions(other.vertexInputAttributeDescriptions), inputAssemblyStateCreateInfo(other.inputAssemblyStateCreateInfo), tessellationStateCreateInfo(other.tessellationStateCreateInfo),
      viewportStateCreateInfo(other.viewportStateCreateInfo), dynamicViewportCount(other.dynamicViewportCount), dynamicScissorCount(other.dynamicScissorCount), rasterizationStateCreateInfo(other.rasterizationStateCreateInfo),
      multisampleStateCreateInfo(other.multisampleStateCreateInfo), depthStencilStateCreateInfo(other.depthStencilStateCreateInfo), colorBlendStateCreateInfo(other.colorBlendStateCreateInfo), colorBlendAttachmentStates(other.colorBlendAttachmentStates),
      dynamicStateCreateInfo(other.dynamicStateCreateInfo), dynamicStates(other.dynamicStates) {
    setCreateInfos();
    updateAllArrayAddresses();
}

GraphicsPipelineCreateInfoPack::operator VkGraphicsPipelineCreateInfo&() { return createInfo; }

void GraphicsPipelineCreateInfoPack::updateAllArrays() {
    createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size());
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
    viewportStateCreateInfo.viewportCount = viewports.size() ? static_cast<uint32_t>(viewports.size()) : dynamicViewportCount;
    viewportStateCreateInfo.scissorCount = scissors.size() ? static_cast<uint32_t>(scissors.size()) : dynamicScissorCount;
    colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

    updateAllArrayAddresses();
}

void GraphicsPipelineCreateInfoPack::setCreateInfos() {
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;
    createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    createInfo.pTessellationState = &tessellationStateCreateInfo;
    createInfo.pViewportState = &viewportStateCreateInfo;
    createInfo.pRasterizationState = &rasterizationStateCreateInfo;
    createInfo.pMultisampleState = &multisampleStateCreateInfo;
    createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    createInfo.pColorBlendState = &colorBlendStateCreateInfo;
    createInfo.pDynamicState = &dynamicStateCreateInfo;
}

void GraphicsPipelineCreateInfoPack::updateAllArrayAddresses() {
    createInfo.pStages = shaderStages.data();
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
    viewportStateCreateInfo.pViewports = viewports.data();
    viewportStateCreateInfo.pScissors = scissors.data();
    colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentStates.data();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
}