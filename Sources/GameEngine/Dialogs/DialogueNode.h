#pragma once
#include <string>
#include <vector>
#include "DialogueOption.h"

namespace GameEngine
{
struct DialogueNode {
    int id;
    std::string npcText;       // Co mówi NPC
    std::vector<DialogueOption> options;
};
}  // namespace GameEngine