#pragma once
#include <string>
#include <filesystem>

namespace GameEngine
{
struct ConfigurationReader
{
    ConfigurationReader();
    ConfigurationReader(const std::string&);
};
std::filesystem::path getGlobalConfigDirPath();
}  // namespace GameEngine
