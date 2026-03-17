#pragma once
#include <Utils/EngineApi.h>

#include <limits>
#include <string>
#include <vector>

#include "DialogueOption.h"
namespace GameEngine
{
struct ENGINE_API DialogueNode
{
    int id{INVALID_NODE_ID};
    int nextNodeID{INVALID_NODE_ID};
    std::string npcText;
    std::string setGameStateflag;
    std::string removeGameStateFlag;
    std::vector<DialogueOption> options;
    std::filesystem::path audioPath;
};
}  // namespace GameEngine