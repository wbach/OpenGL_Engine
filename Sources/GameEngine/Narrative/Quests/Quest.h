#pragma once

#include <Types.h>

#include <optional>
#include <string>
#include <vector>

namespace GameEngine
{
struct QuestAction
{
    std::string type;
    std::vector<std::string> params;
};

struct QuestStep
{
    std::string stepId;
    std::string triggerFlag;
    int requiredValue = 1;
    std::vector<std::string> forbiddenFlags;
    std::vector<QuestAction> actions;
    bool isCompleted = false;
};

struct Quest
{
    std::string name;
    std::vector<QuestStep> steps;
    size_t currentStepIndex = 0;
    bool isActive           = false;
    std::optional<IdType> activeObserverId;

    QuestStep* getCurrentStep()
    {
        if (currentStepIndex < steps.size())
            return &steps[currentStepIndex];
        return nullptr;
    }
};
}  // namespace GameEngine
