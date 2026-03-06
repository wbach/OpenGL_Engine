#pragma once
#include <string>
#include <unordered_map>

namespace GameEngine
{
struct GameState
{
    std::unordered_map<std::string, int> flags;
};
}  // namespace GameEngine
