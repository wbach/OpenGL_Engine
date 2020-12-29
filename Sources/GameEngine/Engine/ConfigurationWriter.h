#pragma once
#include <string>

namespace GameEngine
{
struct Configuration;
void WriteConfigurationToFile(const Configuration&);
void WriteConfigurationToFile(const Configuration&, const std::string&);
}  // namespace GameEngine
