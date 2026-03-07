#pragma once
#include <string>
#include <EngineApi.h>
namespace GameEngine
{
struct ENGINE_API DialogueOption
{
    std::string text;
    int nextNodeID;
    std::string actionFlag;
};
}  // namespace GameEngine