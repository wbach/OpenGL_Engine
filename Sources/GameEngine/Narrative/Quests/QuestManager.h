#pragma once
#include <Types.h>

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "EngineApi.h"
#include "Quest.h"

namespace GameEngine
{
class GameState;
class ISceneManager;
class File;

using ActionFunc = std::function<void(const std::vector<std::string>&)>;

class ENGINE_API QuestManager
{
public:
    QuestManager(GameState&, ISceneManager&);

    void registerAction(const std::string&, ActionFunc);
    void readMainFile(const GameEngine::File&);
    void readQuest(const GameEngine::File&);

    void sceneChanged();
    void update(Quest&);

private:
    void registerDefaultActions();
    void executeStepActions(QuestStep&);
    bool isStepNotForbidden(const QuestStep&) const;

    GameState& gameState;
    ISceneManager& sceneManager;
    std::list<Quest> quests;
    std::unordered_map<std::string, ActionFunc> actionMap;
    std::vector<IdType> activeObservers;
};

}  // namespace GameEngine
