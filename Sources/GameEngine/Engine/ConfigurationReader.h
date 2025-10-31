#pragma once
#include <filesystem>

namespace GameEngine
{
struct ConfigurationReader
{
    ConfigurationReader();
    ConfigurationReader(const std::filesystem::path&);
};
std::filesystem::path getGlobalConfigDirPath();
}  // namespace GameEngine
