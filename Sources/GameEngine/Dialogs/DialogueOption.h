#pragma once
#include <EngineApi.h>

#include <filesystem>
#include <string>
#include <vector>
namespace GameEngine
{
inline constexpr int INVALID_NODE_ID{std::numeric_limits<int>::max()};

enum ENGINE_API class ConditionType
{
    REQUIRED,
    FORBIDDEN
};

struct ENGINE_API DialogueCondition
{
    std::string flag;
    ConditionType type;
};

struct ENGINE_API DialogueOption
{
    std::string text;
    int nextNodeID;
    std::string actionFlag;
    std::vector<DialogueCondition> conditions;
    std::filesystem::path audioPath;
};
}  // namespace GameEngine