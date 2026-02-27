#pragma once
#include <filesystem>
#include <EngineApi.h>

namespace GameEngine
{
struct GlobalConfiguration;
struct LocalConfiguration;

ENGINE_API void WriteConfigurationToFile(const GlobalConfiguration&);
ENGINE_API void WriteConfigurationToFile(const GlobalConfiguration&, const std::filesystem::path&);
ENGINE_API void WriteConfigurationToFile(const LocalConfiguration&, const std::filesystem::path&);
}  // namespace GameEngine
