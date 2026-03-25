#pragma once

#ifndef _SHADER_MODULE_HPP_
#define _SHADER_MODULE_HPP_

#include "../util/Defines.hpp"
#include "../util/Result.hpp"
#include <vulkan/vulkan.h>

class ShaderModule {
private:
    VkShaderModule handle;

public:
    ShaderModule();
    ShaderModule(const VkShaderModuleCreateInfo& createInfo);
    ShaderModule(const char* filePath);
    ShaderModule(size_t codeSize, const uint32_t* pCode);
    ShaderModule(ShaderModule&& other) noexcept;
    ~ShaderModule();

    DEFINE_HANDLE_TYPE_OPERATOR;
    DEFINE_ADDRESS_FUNCTION;

    VkPipelineShaderStageCreateInfo stageCreateInfo(VkShaderStageFlagBits stage, const char* entry = "main") const;

    Result create(VkShaderModuleCreateInfo& createInfo);
    Result create(const char* filePath);
    Result create(size_t codeSize, const uint32_t* pCode);
};

#endif // _SHADER_MODULE_HPP_