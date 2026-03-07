#pragma once
#include <string>
#include <vector>
#include "DialogueOption.h"
#include <Utils/EngineApi.h>
namespace GameEngine
{
struct ENGINE_API DialogueNode {
    int id;
    std::string npcText;       // Co mówi NPC
    std::vector<DialogueOption> options;
};
}  // namespace GameEngine