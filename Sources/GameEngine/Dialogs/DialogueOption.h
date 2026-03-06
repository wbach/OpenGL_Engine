#pragma once
#include <string>

namespace GameEngine
{
struct DialogueOption
{
    std::string text;
    int nextNodeID;
    std::string actionFlag;
};
}  // namespace GameEngine