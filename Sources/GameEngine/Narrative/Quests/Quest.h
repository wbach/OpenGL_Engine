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
    std::vector<std::string> descriptionNotes{
        "Please, hero. I'm glad you're here.They came in the night and took the life of some of the villagers. Friends, family, "
        "husbands and wives, gone in a flash. We know who did it, but we lack the power to avenge our fallen. Please, hero, be "
        "our vengeance and unleash our fury upon those nasty dwarves. I cannot join you, for I have other tasks to attend to, "
        "but I know you don't need me anyway.Hero, act swiftly when dealing with the dwarves. Try to kill all those who stand in "
        "your way, we don't need their filth in our lands.There's no big reward at the end of the line, but I think my reward "
        "will be more than fair. For justice and honor!"};
    std::vector<QuestStep> steps;
    size_t currentStepIndex = 0;
    bool isActive           = false;
    std::optional<IdType> activeObserverId;
    std::string activationFlag;

    QuestStep* getCurrentStep()
    {
        if (currentStepIndex < steps.size())
            return &steps[currentStepIndex];
        return nullptr;
    }
};
}  // namespace GameEngine
