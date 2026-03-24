#include "Framebuffer.hpp"
#include "VulkanBase.hpp"
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

Framebuffer::Framebuffer() = default;
Framebuffer::Framebuffer(const VkFramebufferCreateInfo& createInfo) {
    VkFramebufferCreateInfo info = createInfo;
    create(info);
}
Framebuffer::Framebuffer(Framebuffer&& other) noexcept { MOVE_HANDLE; }
Framebuffer::~Framebuffer() { DESTROY_HANDLE_BY(vkDestroyFramebuffer); }

Result Framebuffer::create(VkFramebufferCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    if (Result result = vkCreateFramebuffer(VulkanBase::getVulkanBase().getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create framebuffer: " << string_VkResult(result) << std::endl;
        return result;
    }
    return VK_SUCCESS;
}