#pragma once
#include <Utils/EngineApi.h>

#include <string>
#include <vector>

#include "DialogueOption.h"
namespace GameEngine
{
struct ENGINE_API DialogueNode
{
    int id;
    std::string npcText;  // Co mówi NPC
    std::vector<DialogueOption> options;
};
}  // namespace GameEngine