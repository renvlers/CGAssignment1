#pragma once

#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "RenderPassWithFrameBuffers.hpp"
#include "VulkanBase.hpp"
#include <vulkan/vulkan.h>

extern PipelineLayout pipelineLayout;
extern Pipeline pipeline;

inline const auto& createRpwfScreen() {
    static RenderPassWithFramebuffers rpwf;
    static VkFormat colorAttachmentFormat;

    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();

    constexpr auto createRenderPass = [] {
        VkAttachmentDescription attachmentDescription{
            .format = colorAttachmentFormat = VulkanBase::getVulkanBase().getSwapchainCreateInfo().imageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference attachmentReference{
            0,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpassDescription{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentReference,
        };

        VkSubpassDependency subpassDependency{
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        VkRenderPassCreateInfo renderPassCreateInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &attachmentDescription,
            .subpassCount = 1,
            .pSubpasses = &subpassDescription,
            .dependencyCount = 1,
            .pDependencies = &subpassDependency,
        };
        rpwf.renderPass.create(renderPassCreateInfo);
    };

    createRenderPass();

    auto createFramebuffers = [&vulkanBase] {
        rpwf.framebuffers.resize(vulkanBase.getSwapchainImageCount());

        VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = rpwf.renderPass,
            .attachmentCount = 1,
            .width = vulkanBase.getSwapchainCreateInfo().imageExtent.width,
            .height = vulkanBase.getSwapchainCreateInfo().imageExtent.height,
            .layers = 1,
        };

        for (size_t i = 0; i < vulkanBase.getSwapchainImageCount(); i++) {
            VkImageView attachment = vulkanBase.getSwapchainImageView(i);

            framebufferCreateInfo.pAttachments = &attachment;
            rpwf.framebuffers[i].create(framebufferCreateInfo);
        }
    };

    createFramebuffers();

    auto destroyFramebuffers = [] { rpwf.framebuffers.clear(); };

    EXECUTE_ONCE(rpwf);

    vulkanBase.addCreateSwapchainCallback([&] {
        if (colorAttachmentFormat != vulkanBase.getSwapchainCreateInfo().imageFormat) {
            colorAttachmentFormat = vulkanBase.getSwapchainCreateInfo().imageFormat;
            rpwf.renderPass.~RenderPass();
            createRenderPass();
        }
    });
    vulkanBase.addCreateSwapchainCallback(createFramebuffers);

    vulkanBase.addDestroySwapchainCallback(destroyFramebuffers);

    return rpwf;
}

inline const auto& getRpwf() {
    static const auto& rpwf = createRpwfScreen();
    return rpwf;
}

void createLayout();
void createPipeline();

#endif // _FUNCTIONS_HPP_