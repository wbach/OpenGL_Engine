#pragma once
#include <string>

namespace GameEngine
{
struct Configuration;
void WriteConfigurationToFile(const Configuration& configuration, const std::string& filename);
}  // namespace GameEngine
