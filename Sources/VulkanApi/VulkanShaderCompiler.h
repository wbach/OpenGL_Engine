#pragma once
#include <shaderc/shaderc.hpp>
#include <string>

namespace GraphicsApi::Vulkan
{
std::vector<uint32_t> CompileGlslToSpirv(const std::string&, shaderc_shader_kind, const std::string&);
}  // namespace GraphicsApi::Vulkan
