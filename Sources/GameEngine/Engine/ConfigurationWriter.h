#pragma once
#include <filesystem>

namespace GameEngine
{
struct Configuration;
void WriteConfigurationToFile(const Configuration&);
void WriteConfigurationToFile(const Configuration&, const std::filesystem::path&);
}  // namespace GameEngine

