#pragma once
#include <string>

namespace GameEngine
{
struct Configuration;
void ReadConfiguration(Configuration& configuration, const std::string& filename);
}  // namespace GameEngine
