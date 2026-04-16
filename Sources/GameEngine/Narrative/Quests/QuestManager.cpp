#include "QuestManager.h"

#include <Utils/Json/JsonReader.h>

#include <optional>
#include <string>
#include <vector>

#include "GameEngine/Components/Controllers/AIController.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Narrative/Dialogs/DialogueManager.h"
#include "GameEngine/Narrative/GameState.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Scene/ISceneManager.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Types.h"
#include "Utils.h"

namespace GameEngine
{
QuestManager::QuestManager(GameState& gs, ISceneManager& sceneManager, std::function<void(EngineEvent)> addEngineEvent)
    : gameState(gs)
    , sceneManager(sceneManager)
    , addEngineEvent(addEngineEvent)
{
    registerDefaultActions();

    const auto& questFilePath = EngineLocalConf.files.getQuestsFilePath();
    if (not questFilePath.empty())
    {
        File questFile(questFilePath);

        if (questFile.exist())
        {
            readMainFile(questFile);
        }
        else
        {
            LOG_WARN << "Main quest file not exist : " << questFile;
        }
    }
    else
    {
        LOG_DEBUG << "No quest file is set.";
    }
}

void QuestManager::registerDefaultActions()
{
    registerAction("log",
                   [](const auto& params)
                   {
                       if (!params.empty())
                       {
                           LOG_DEBUG << "[QUEST LOG]: " << params;
                       }
                   });

    registerAction("setFlag",
                   [this](const auto& params)
                   {
                       try
                       {
                           if (params.size() >= 2)
                           {
                               addEngineEvent(SetGameStateFlag{.flag = params[0], .value = std::stoi(params[1])});
                           }
                       }
                       catch (...)
                       {
                           LOG_WARN << "Parse error";
                       }
                   });

    registerAction("moveToPos",
                   [&](const std::vector<std::string>& params)
                   {
                       if (params.size() < 2)
                       {
                           LOG_DEBUG << "not enough arguments";
                           return;
                       }

                       const std::string& gameObjectName = params[0];

                       vec3 target(0);
                       if (not std::from_string(params[1], target))
                       {
                           LOG_DEBUG << "Target pos parse error.";
                           return;
                       }

                       if (auto scene = sceneManager.GetActiveScene())
                       {
                           if (auto go = scene->GetGameObject(gameObjectName))
                           {
                               if (auto ai = go->GetComponent<Components::AIController>())
                               {
                                   ai->MoveTo(target);
                               }
                               else
                               {
                                   LOG_DEBUG << "ai controller not found : " << gameObjectName;
                               }
                           }
                           else
                           {
                               LOG_DEBUG << "gameobject not found : " << gameObjectName;
                           }
                       }
                   });

    registerAction("moveToObject",
                   [&](const std::vector<std::string>& params)
                   {
                       if (params.size() < 2)
                       {
                           LOG_DEBUG << "not enough arguments";
                           return;
                       }

                       const std::string& targetObjectName = params[1];
                       const std::string& gameObjectName   = params[0];

                       LOG_DEBUG << "GameObject: " << gameObjectName << ", targetObjectName " << targetObjectName;

                       if (auto scene = sceneManager.GetActiveScene())
                       {
                           LOG_DEBUG << "GetGameObject " << gameObjectName;
                           if (auto go = scene->GetGameObject(gameObjectName))
                           {
                               LOG_DEBUG << "GetComponent " << gameObjectName;
                               if (auto ai = go->GetComponent<Components::AIController>())
                               {
                                   LOG_DEBUG << "GetGameObject " << targetObjectName;
                                   if (auto trargetGo = scene->GetGameObject(targetObjectName))
                                   {
                                       ai->MoveTo(trargetGo->GetWorldTransform().GetPosition());
                                   }
                                   else
                                   {
                                       LOG_DEBUG << "target game object not found: " << trargetGo;
                                   }
                               }
                               else
                               {
                                   LOG_DEBUG << "ai controller not found : " << gameObjectName;
                               }
                           }
                           else
                           {
                               LOG_DEBUG << "gameobject not found : " << gameObjectName;
                           }
                       }
                       else
                       {
                           LOG_DEBUG << "No active scene";
                       }
                   });

    registerAction("startDialog",
                   [&](const std::vector<std::string>& params)
                   {
                       if (params.size() < 2)
                           return;

                       const std::string& playerName = params[0];
                       const std::string& npcName    = params[1];
                       const std::string& dialogFile = params[2];

                       if (auto scene = sceneManager.GetActiveScene())
                       {
                           if (auto player = scene->GetGameObject(playerName))
                           {
                               if (auto npc = scene->GetGameObject(npcName))
                               {
                                   auto& component        = npc->AddComponent<Components::DialogueComponent>();
                                   component.dialogueFile = dialogFile;
                                   component.readFile();

                                   if (auto dialogManager = scene->GetDialogueManager())
                                   {
                                       LOG_DEBUG << "startDialogue";
                                       dialogManager->startDialogue(*player, component);
                                       LOG_DEBUG << "startDialogue end";
                                   }

                                   //    // Add dialog component with based on  params[2]?
                                   //    if (auto maybeDialogComponent = npc->GetComponent<Components::DialogueComponent>())
                                   //    {
                                   //        if (auto dialogManager = scene->GetDialogueManager())
                                   //            dialogManager->startDialogue(*player, *maybeDialogComponent);
                                   //    }
                                   //    else
                                   //    {
                                   //        LOG_WARN << "Npc not have dialog component." << npcName;
                                   //    }
                               }
                               else
                               {
                                   LOG_WARN << "Npc not found. GameObject name : " << npcName;
                               }
                           }
                           else
                           {
                               LOG_WARN << "Player not found. GameObject name : " << playerName;
                           }
                       }
                       else
                       {
                           LOG_WARN << "No active scene.";
                       }
                   });
}
void QuestManager::onSceneStarted()
{
    for (auto& quest : quests)
    {
        update(quest);
    }
}
void QuestManager::update(Quest& quest)
{
    if (not quest.isActive)
        return;

    auto* step = quest.getCurrentStep();
    if (not step or step->isCompleted)
        return;

    auto shouldTriggerActions =
        step->triggerFlag.empty() or gameState.getFlag(step->triggerFlag).value_or(0) >= step->requiredValue;

    if (isStepNotForbidden(*step) and shouldTriggerActions)
    {
        executeStepActions(*step);
        quest.currentStepIndex++;

        if (quest.activeObserverId)
        {
            gameState.removeObserver(*quest.activeObserverId);
            quest.activeObserverId.reset();
        }

        update(quest);
        return;
    }

    if (isStepNotForbidden(*step) and not quest.activeObserverId.has_value())
    {
        quest.activeObserverId =
            gameState.addObserver(step->triggerFlag, [this, &quest](const std::string&, int val) { this->update(quest); });
    }
}

bool QuestManager::isStepNotForbidden(const QuestStep& step) const
{
    for (const auto& f : step.forbiddenFlags)
    {
        if (gameState.hasFlag(f))
            return false;
    }
    return true;
}

void QuestManager::executeStepActions(QuestStep& step)
{
    for (const auto& action : step.actions)
    {
        if (actionMap.contains(action.type))
        {
            actionMap[action.type](action.params);
        }
    }
    step.isCompleted = true;
}

void QuestManager::readQuest(const GameEngine::File& file)
{
    Utils::JsonReader reader;
    if (not file.exist() or not reader.Read(file.GetAbsolutePath().string()))
    {
        LOG_DEBUG << "Read quest file error: " << file;
        return;
    }

    Quest quest;
    if (auto nameNode = reader.Get("name"))
        quest.name = nameNode->value_;
    if (auto activeNode = reader.Get("isActive"))
        quest.isActive = Utils::StringToBool(activeNode->value_);
    if (auto descriptionNotesNode = reader.Get("descriptionNotes"))
    {
        quest.descriptionNotes.clear();
        for (const auto& childNode : descriptionNotesNode->getChildren())
        {
            LOG_DEBUG << childNode->name();
            LOG_DEBUG << childNode->value_;
            quest.descriptionNotes.push_back(childNode->value_);
        }
    }

    if (auto stepsNode = reader.Get("steps"))
    {
        for (const auto& stepNode : stepsNode->getChildren())
        {
            QuestStep step;
            if (auto idNode = stepNode->getChild("stepId"))
                step.stepId = idNode->value_;
            if (auto trigNode = stepNode->getChild("trigger"))
                step.triggerFlag = trigNode->value_;
            if (auto valNode = stepNode->getChild("value"))
                step.requiredValue = std::stoi(valNode->value_);

            if (auto forbiddenNode = stepNode->getChild("forbiddenFlags"))
            {
                for (const auto& fNode : forbiddenNode->getChildren())
                    step.forbiddenFlags.push_back(fNode->value_);
            }

            if (auto actionsNode = stepNode->getChild("actions"))
            {
                for (const auto& actionNode : actionsNode->getChildren())
                {
                    QuestAction action;
                    if (auto typeNode = actionNode->getChild("type"))
                        action.type = typeNode->value_;

                    if (auto paramsNode = actionNode->getChild("params"))
                    {
                        for (const auto& pNode : paramsNode->getChildren())
                            action.params.push_back(pNode->value_);
                    }
                    step.actions.push_back(action);
                }
            }
            quest.steps.push_back(step);
        }
    }

    quests.push_back(std::move(quest));
}

void QuestManager::readMainFile(const GameEngine::File& file)
{
    LOG_DEBUG << "Read quest main file: " << file;

    Utils::JsonReader reader;
    if (not file.exist() or not reader.Read(file.GetAbsolutePath().string()))
    {
        LOG_DEBUG << "Read quests file error:  " << file;
        return;
    }

    if (auto questsListNode = reader.Get("quests"))
    {
        for (const auto& filesNode : questsListNode->getChildren())
        {
            if (auto fileField = filesNode->getChild("file"))
            {
                File questFile(fileField->value_);
                readQuest(questFile);
            }
        }
    }
}

void QuestManager::registerAction(const std::string& type, ActionFunc action)
{
    actionMap[type] = action;
}
const QuestManager::Quests& QuestManager::getQuests() const
{
    return quests;
}
}  // namespace GameEngine
