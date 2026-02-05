#pragma once
#include <filesystem>

namespace GameEngine
{
struct GlobalConfiguration;
struct LocalConfiguration;

void ReadConfigFromFile(GlobalConfiguration&);
void ReadConfigFromFile(LocalConfiguration&, const std::filesystem::path&);

std::filesystem::path getGlobalConfigDirPath();
}  // namespace GameEngine
