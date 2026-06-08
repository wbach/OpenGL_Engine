#pragma once
#include <shaderc/shaderc.hpp>
#include <string>

namespace GraphicsApi::Vulkan
{
std::string ReadTextFile(const std::string&);
std::vector<uint32_t> CompileGlslToSpirv(const std::string&, shaderc_shader_kind, const std::string&);
}  // namespace GraphicsApi::Vulkan
