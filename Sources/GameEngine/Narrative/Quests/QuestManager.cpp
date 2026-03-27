#include "QuestManager.h"

#include <Utils/Json/JsonReader.h>

#include <optional>
#include <string>

#include "GameEngine/Components/Controllers/AIController.h"
#include "GameEngine/Narrative/GameState.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/ISceneManager.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Types.h"
#include "Utils.h"

namespace GameEngine
{
QuestManager::QuestManager(GameState& gs, ISceneManager& sceneManager)
    : gameState(gs)
    , sceneManager(sceneManager)
{
    start();
}
void QuestManager::registerActions()
{
    auto moveTo = [&](const std::vector<std::string>& params)
    {
        if (params.size() < 2)
            return;

        vec3 target(0);
        if (not std::from_string(params[1], target))
            return;

        if (auto scene = sceneManager.GetActiveScene())
        {
            if (auto go = scene->GetGameObject(params[0]))
            {
                if (auto ai = go->GetComponent<Components::AIController>())
                {
                    ai->MoveTo(target);
                }
            }
        }
    };
    actions = {{"moveTo", moveTo}};
}
void QuestManager::onFlagChanged(const std::string& flag, int value)
{
}
void QuestManager::start()
{
}
void QuestManager::readQuest(const GameEngine::File& file)
{
    Utils::JsonReader reader;
    if (not file.exist())
    {
        LOG_DEBUG << "Main file not exist: " << file;
        return;
    }

    if (not reader.Read(file.GetAbsolutePath().string()))
    {
        LOG_DEBUG << "Read file error: " << file;
        return;
    }

    Quest result;
    bool isActive{false};
    if (auto nodesNode = reader.Get("isActive"))
    {
        isActive = Utils::StringToBool(nodesNode->value_);
    }

    quests.push_back(std::move(result));
    if (isActive)
    {
        activeQuests.push_back(quests.size() - 1);
    }
}
void QuestManager::readMainFile(const GameEngine::File& file)
{
    Utils::JsonReader reader;
    if (not file.exist())
    {
        LOG_DEBUG << "Main file not exist: " << file;
        return;
    }

    if (not reader.Read(file.GetAbsolutePath().string()))
    {
        LOG_DEBUG << "Read file error: " << file;
        return;
    }

    if (auto nodesNode = reader.Get("quests"))
    {
        for (const auto& nodeNode : nodesNode->getChildren())
        {
            File file;
            ::Read(nodeNode->getChild("file"), file);
            readQuest(file);
        }
    }
}
}  // namespace GameEngine
