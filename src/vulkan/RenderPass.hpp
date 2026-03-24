#pragma once

#ifndef _RENDER_PASS_HPP_
#define _RENDER_PASS_HPP_

#include "../util/ArrayRef.hpp"
#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class RenderPass {
private:
    VkRenderPass handle;

public:
    RenderPass();
    RenderPass(const VkRenderPassCreateInfo& createInfo);
    RenderPass(RenderPass&& other) noexcept;
    ~RenderPass();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    void cmdBegin(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) const;
    void cmdBegin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRect2D renderArea, ArrayRef<const VkClearValue> clearValues = {}, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) const;

    void cmdNext(VkCommandBuffer commandBuffer, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) const;

    void cmdEnd(VkCommandBuffer commandBuffer) const;

    Result create(VkRenderPassCreateInfo& createInfo);
};

#endif // _RENDER_PASS_HPP_