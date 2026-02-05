#pragma once
#include <filesystem>

namespace GameEngine
{
struct GlobalConfiguration;
struct LocalConfiguration;

void WriteConfigurationToFile(const GlobalConfiguration&);
void WriteConfigurationToFile(const GlobalConfiguration&, const std::filesystem::path&);
void WriteConfigurationToFile(const LocalConfiguration&, const std::filesystem::path&);
}  // namespace GameEngine
