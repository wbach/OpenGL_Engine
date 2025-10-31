#pragma once
#include <filesystem>

namespace GameEngine
{
struct Configuration;
void ReadConfiguration(Configuration& configuration, const std::filesystem::path& filename);
}  // namespace GameEngine
