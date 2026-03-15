#pragma once
#include <filesystem>
#include <string>
#include <EngineApi.h>
namespace GameEngine
{
struct ENGINE_API DialogueOption
{
    std::string text;
    int nextNodeID;
    std::string actionFlag;
    std::filesystem::path audioPath;
};
}  // namespace GameEngine