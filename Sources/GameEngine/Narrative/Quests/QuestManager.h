#pragma once
#include <EngineApi.h>

#include <cstddef>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Types.h"

namespace GameEngine
{
class GameState;
class ISceneManager;
class File;

struct QuestAction
{
    std::string type;
    std::vector<std::string> params;
};

struct QuestObjective
{
    std::string description;
    std::string targetKey;

    int currentAmount = 0;
    int requiredAmount;
    bool isReached = false;

    bool update(int amount)
    {
        currentAmount += amount;
        if (currentAmount >= requiredAmount)
            isReached = true;
        return isReached;
    }
};

struct QuestStep
{
    std::string stepName;
    std::string triggerFlag;
    int requiredValue;

    std::vector<QuestAction> actions;
    std::vector<QuestObjective> objectives;

    bool checkCompletion()
    {
        for (const auto& obj : objectives)
        {
            if (!obj.isReached)
                return false;
        }
        return true;
    }
};

struct Quest
{
    std::string name;
    std::vector<QuestStep> steps;
    size_t currentStepIndex = 0;
};

class ENGINE_API QuestManager
{
public:
    using Action  = std::function<void(const std::vector<std::string>&)>;
    using Actions = std::unordered_map<std::string, Action>;

    QuestManager(GameState&, ISceneManager&);

    void onFlagChanged(const std::string&, int);
    void start();

private:
    void registerActions();
    void readMainFile(const GameEngine::File&);
    void readQuest(const GameEngine::File&);

private:
    GameState& gameState;
    ISceneManager& sceneManager;
    Actions actions;

    std::vector<Quest> quests;
    std::vector<size_t> activeQuests;
};
}  // namespace GameEngine
