#pragma once

#include <filesystem>
#include <vulkan/vulkan.h>

#include "GraphicsApi/GraphicsApiDef.h"
#include "GraphicsApi/ShaderProgramType.h"
#include "Types.h"
#include "VulkanApi/VulkanContext.hpp"
#include "VulkanProgram.h"

namespace GraphicsApi::Vulkan
{
class VulkanShaderManager
{
public:
    explicit VulkanShaderManager(VulkanContext& vkContext);

    void SetShadersFilesLocations(const std::filesystem::path& path);
    ID Create(ShaderProgramType type);

private:
    VulkanContext& vkContext_;
    std::filesystem::path shadersFileLocation_;
};
}  // namespace GraphicsApi::Vulkan
