#pragma once
#include <EngineApi.h>

#include <filesystem>
#include <string>
namespace GameEngine
{
inline constexpr int INVALID_NODE_ID{std::numeric_limits<int>::max()};
struct ENGINE_API DialogueOption
{
    std::string text;
    int nextNodeID;
    std::string actionFlag;
    std::filesystem::path audioPath;
};
}  // namespace GameEngine