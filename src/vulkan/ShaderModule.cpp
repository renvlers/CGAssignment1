#include "ShaderModule.hpp"
#include "VulkanBase.hpp"
#include <fstream>
#include <iostream>
#include <vulkan/vk_enum_string_helper.h>

ShaderModule::ShaderModule() = default;
ShaderModule::ShaderModule(const VkShaderModuleCreateInfo& createInfo) {
    VkShaderModuleCreateInfo into = createInfo;
    create(into);
}
ShaderModule::ShaderModule(const char* filePath) { create(filePath); }
ShaderModule::ShaderModule(size_t codeSize, const uint32_t* pCode) { create(codeSize, pCode); }
ShaderModule::ShaderModule(ShaderModule&& other) noexcept { MOVE_HANDLE; }
ShaderModule::~ShaderModule() { DESTROY_HANDLE_BY(vkDestroyShaderModule); }

VkPipelineShaderStageCreateInfo ShaderModule::stageCreateInfo(VkShaderStageFlagBits stage, const char* entry) const {
    return VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = stage,
        .module = handle,
        .pName = entry,
    };
}

Result ShaderModule::create(VkShaderModuleCreateInfo& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    VulkanBase& vulkanBase = VulkanBase::getVulkanBase();
    if (Result result = vkCreateShaderModule(vulkanBase.getDevice(), &createInfo, nullptr, &handle); result != VK_SUCCESS) {
        std::cerr << "Failed to create shader module: " << string_VkResult(result) << std::endl;
        return result;
    }

    return VK_SUCCESS;
}

Result ShaderModule::create(const char* filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return VK_RESULT_MAX_ENUM;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> binaries(fileSize / sizeof(uint32_t));
    file.seekg(0);
    file.read(reinterpret_cast<char*>(binaries.data()), fileSize);
    file.close();

    return create(binaries.size() * sizeof(uint32_t), binaries.data());
}

Result ShaderModule::create(size_t codeSize, const uint32_t* pCode) {
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = codeSize,
        .pCode = pCode,
    };

    return create(createInfo);
}