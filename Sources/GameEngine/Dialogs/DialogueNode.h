#pragma once
#include <Utils/EngineApi.h>

#include <limits>
#include <string>
#include <vector>

#include "DialogueOption.h"
namespace GameEngine
{
inline constexpr int INVALID_NODE_ID{std::numeric_limits<int>::max()};
struct ENGINE_API DialogueNode
{
    int id{INVALID_NODE_ID};
    std::string npcText;
    std::vector<DialogueOption> options;
    std::filesystem::path audioPath;
};
}  // namespace GameEngine