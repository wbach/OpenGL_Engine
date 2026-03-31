#pragma once
#include <Types.h>

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "EngineApi.h"
#include "GameEngine/Engine/EngineEvent.h"
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
    using Quests = std::list<Quest>;

    QuestManager(GameState&, ISceneManager&, std::function<void(EngineEvent)>);

    void registerAction(const std::string&, ActionFunc);
    void readMainFile(const GameEngine::File&);
    void readQuest(const GameEngine::File&);

    void onSceneStarted();
    void update(Quest&);

    const Quests& getQuests() const;

private:
    void registerDefaultActions();
    void executeStepActions(QuestStep&);
    bool isStepNotForbidden(const QuestStep&) const;

    GameState& gameState;
    ISceneManager& sceneManager;
    Quests quests;
    std::unordered_map<std::string, ActionFunc> actionMap;
    std::vector<IdType> activeObservers;
    std::function<void(EngineEvent)> addEngineEvent;
};

}  // namespace GameEngine
