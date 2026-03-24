#include "RenderPass.hpp"
#include "../util/Defines.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

RenderPass::RenderPass() = default;
RenderPass::RenderPass(const VkRenderPassCreateInfo& createInfo) {
    VkRenderPassCreateInfo info = createInfo;
    create(info);
}
RenderPass::RenderPass(RenderPass&& other) noexcept { MOVE_HANDLE; }
RenderPass::~RenderPass() { DESTROY_HANDLE_BY(vkDestroyRenderPass); }

void RenderPass::cmdBegin(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents) const {
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = handle;
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, contents);
}

void RenderPass::cmdBegin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValues, VkSubpassContents contents) const {
    VkRenderPassBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = handle,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.pointer(),
    };
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, contents);
}

void RenderPass::cmdNext(VkCommandBuffer commandBuffer, VkSubpassContents contents) const { vkCmdNextSubpass(commandBuffer, contents); }

void RenderPass::cmdEnd(VkCommandBuffer commandBuffer) const { vkCmdEndRenderPass(commandBuffer); }

Result RenderPass::create(VkRenderPassCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    if (Result result = vkCreateRenderPass(VulkanBase::getVulkanBase().getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create render pass: " << string_VkResult(result) << std::endl;
        return result;
    }
    return VK_SUCCESS;
}