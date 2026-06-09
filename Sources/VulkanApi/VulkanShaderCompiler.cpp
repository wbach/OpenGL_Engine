#include "VulkanShaderCompiler.h"

#include <Types.h>
#include <vulkan/vulkan.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace GraphicsApi::Vulkan
{
std::string ReadTextFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (not file.is_open())
    {
        std::cerr << "Nie udalo sie otworzyc pliku: " << filename << "\n";
        return "";
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

std::vector<uint32_t> CompileGlslToSpirv(const std::string& sourceName, shaderc_shader_kind kind, const std::string& sourceStr)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(sourceStr, kind, sourceName.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << "Blad kompilacji shaderow (" << sourceName << "):\n" << result.GetErrorMessage() << "\n";
        return std::vector<uint32_t>();
    }
    return std::vector<uint32_t>(result.cbegin(), result.cend());
}
}  // namespace GraphicsApi::Vulkan
