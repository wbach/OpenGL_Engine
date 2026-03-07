#pragma once
#include <string>
#include <unordered_map>
#include <EngineApi.h>
namespace GameEngine
{
struct ENGINE_API GameState
{
    std::unordered_map<std::string, int> flags;
};
}  // namespace GameEngine
