#include "Functions.hpp"
#include "../polynomial/PolynomialDrawer.hpp"
#include "GraphicsPipelineCreateInfoPack.hpp"
#include "ShaderModule.hpp"
#include "VulkanBase.hpp"

PipelineLayout pipelineLayout;
Pipeline pipeline;

void createLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
}

void createPipeline() {
    static ShaderModule vert("shader/VertShader.vert.spv");
    static ShaderModule frag("shader/FragShader.frag.spv");
    static VkPipelineShaderStageCreateInfo shaderStages[] = {
        vert.stageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT),
        frag.stageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT),
    };

    static VkVertexInputBindingDescription bindingDescription{
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    static VkVertexInputAttributeDescription attributeDescription{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
    };

    auto create = [] {
        GraphicsPipelineCreateInfoPack pipelineCreateInfoPack;

        pipelineCreateInfoPack.createInfo.layout = pipelineLayout;
        pipelineCreateInfoPack.createInfo.renderPass = getRpwf().renderPass;
        pipelineCreateInfoPack.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        pipelineCreateInfoPack.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        pipelineCreateInfoPack.vertexInputBindingDescriptions.push_back(bindingDescription);
        pipelineCreateInfoPack.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
        pipelineCreateInfoPack.vertexInputAttributeDescriptions.push_back(attributeDescription);
        pipelineCreateInfoPack.viewports.emplace_back(0.0f, 0.0f, static_cast<float>(VulkanBase::getVulkanBase().getSwapchainCreateInfo().imageExtent.width), static_cast<float>(VulkanBase::getVulkanBase().getSwapchainCreateInfo().imageExtent.height));
        pipelineCreateInfoPack.scissors.emplace_back(VkOffset2D{}, VulkanBase::getVulkanBase().getSwapchainCreateInfo().imageExtent);
        pipelineCreateInfoPack.multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineCreateInfoPack.colorBlendAttachmentStates.push_back({.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT});
        pipelineCreateInfoPack.updateAllArrays();
        pipelineCreateInfoPack.createInfo.stageCount = 2;
        pipelineCreateInfoPack.createInfo.pStages = shaderStages;

        pipeline.create(pipelineCreateInfoPack);
    };

    auto destroy = [] { pipeline.~Pipeline(); };

    VulkanBase::getVulkanBase().addCreateSwapchainCallback(create);
    VulkanBase::getVulkanBase().addDestroySwapchainCallback(destroy);

    create();
}