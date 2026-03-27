#include "DialogueComponent.h"

#include <Utils/TreeNode.h>

#include <optional>

#include "Common/Transform.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Dialogs/DialogueOption.h"
#include "GameEngine/Dialogs/GameState.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Json/JsonReader.h"
#include "Logger/Log.h"
namespace GameEngine
{
namespace
{
constexpr char CSTR_DIALOG_FILE[]   = "dialogueFile";
constexpr char CSTR_START_NODE_ID[] = "startNodeID";
}  // namespace

namespace Components
{

DialogueComponent::DialogueComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<DialogueComponent>(), componentContext, gameObject)
{
}
DialogueComponent::~DialogueComponent()
{
    LOG_DEBUG << "";
}
void DialogueComponent::CleanUp()
{
}
void DialogueComponent::Reload()
{
}
void DialogueComponent::ReqisterFunctions()
{
}
void DialogueComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject)
    {
        auto component = std::make_unique<DialogueComponent>(componentContext, gameObject);
        component->read(node);
        std::filesystem::path filePath;
        ::Read(node.getChild(CSTR_DIALOG_FILE), filePath);
        component->dialogueFile = filePath;

        ::Read(node.getChild(CSTR_START_NODE_ID), component->startNodeID);
        // if (component->dialogueFile.exist())
        {
            component->readFile();
        }
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<DialogueComponent>(), func);
}
void DialogueComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(CSTR_DIALOG_FILE), dialogueFile.GetDataRelativePath());
    ::write(node.addChild(CSTR_START_NODE_ID), startNodeID);
}
void DialogueComponent::readFile()
{
    Utils::JsonReader reader;
    if (not dialogueFile.exist())
    {
        LOG_DEBUG << "Dialog file not exist: " << dialogueFile;
        return;
    }

    if (not reader.Read(dialogueFile.GetAbsolutePath().string()))
    {
        LOG_DEBUG << "Read dialog file error: " << dialogueFile;
        return;
    }

    if (auto nodesNode = reader.Get("nodes"))
    {
        for (const auto& nodeNode : nodesNode->getChildren())
        {
            DialogueNode node{};

            ::Read(nodeNode->getChild("id"), node.id);

            if (node.id == INVALID_NODE_ID)
            {
                LOG_WARN << "Can not read node id for node : " << *nodeNode;
                continue;
            }

            ::Read(nodeNode->getChild("text"), node.text);
            ::Read(nodeNode->getChild("audio"), node.audioPath);
            ::Read(nodeNode->getChild("setGameStateflag"), node.setGameStateflag);
            ::Read(nodeNode->getChild("removeGameStateFlag"), node.removeGameStateFlag);
            ::Read(nodeNode->getChild("next"), node.nextNodeID);
            ::Read(nodeNode->getChild("backTo"), node.backToNodeID);

            node.text = Utils::RemovePolishSigns(node.text);

            if (auto optionsNode = nodeNode->getChild("options"))
            {
                for (const auto& optionNode : optionsNode->getChildren())
                {
                    DialogueOption option;

                    ::Read(optionNode->getChild("next"), option.nextNodeID);
                    ::Read(optionNode->getChild("backTo"), option.backToNodeID);
                    ::Read(optionNode->getChild("text"), option.text);
                    ::Read(optionNode->getChild("setGameStateflag"), option.setGameStateflag);
                    ::Read(optionNode->getChild("removeGameStateFlag"), option.removeGameStateFlag);
                    ::Read(optionNode->getChild("audio"), option.audioPath);

                    if (auto condition = optionNode->getChild("condition"))
                    {
                        for (const auto& child : condition->getChildren())
                        {
                            DialogueCondition condition;
                            ::Read(child->getChild("flag"), condition.flag);

                            std::string typeStr;
                            ::Read(child->getChild("type"), typeStr);
                            auto type = magic_enum::enum_cast<ConditionType>(typeStr);

                            LOG_DEBUG << "typeStr " << typeStr << " has enum: " << type.has_value();
                            condition.type = type.value_or(ConditionType::REQUIRED);
                            option.conditions.push_back(condition);
                        }
                    }

                    option.text = Utils::RemovePolishSigns(option.text);

                    node.options.push_back(option);
                }
            }

            nodes[node.id] = std::move(node);
        }
    }
}
const DialogueOption* DialogueComponent::selectOption(size_t optionIndex)
{
    DialogueOption* selected{nullptr};

    auto iter = nodes.find(currentNodeID);
    if (iter != nodes.end())
    {
        LOG_DEBUG << "";
        auto& options = iter->second.options;
        if (optionIndex < options.size())
        {
            selected = &options[optionIndex];
        }
        else
        {
            LOG_WARN << "Options out of range, options.size()= " << options.size() << "optionIndex : " << optionIndex;
            return nullptr;
        }
    }
    else
    {
        LOG_WARN << "Current node not exist in map : " << currentNodeID;
        return nullptr;
    }

    if (not selected->setGameStateflag.empty())
    {
        componentContext_.gamestate.flags[selected->setGameStateflag] = true;
    }
    if (not selected->removeGameStateFlag.empty())
    {
        componentContext_.gamestate.flags[selected->removeGameStateFlag] = false;
        // componentContext_.gamestate.flags.erase(selected->removeGameStateFlag);
    }
    return selected;
}

const DialogueNode* DialogueComponent::getCurrent() const
{
    auto iter = nodes.find(currentNodeID);
    if (iter != nodes.end())
    {
        return &iter->second;
    }

    LOG_DEBUG << "Current not set! currentNodeID = " << currentNodeID << ", nodes :" << nodes;
    return nullptr;
}

void DialogueComponent::RotateObjectToPlayer(const vec3& playerPos)
{
    auto npcPos = thisObject_.GetWorldTransform().GetPosition();

    vec3 diff           = npcPos - playerPos;
    float distanceSq    = dot(diff, diff);
    const float epsilon = 0.0001f;
    if (distanceSq < epsilon)
    {
        LOG_DEBUG << "Player and NPC are in the same position";
        return;
    }

    tmpRotation = thisObject_.GetWorldTransform().GetRotation();

    vec3 targetPosXZ(playerPos.x, npcPos.y, playerPos.z);
    auto dir = targetPosXZ - npcPos;

    if (glm::length(dir) < 0.001f)
    {
        LOG_DEBUG << "Rotate not neded";
        return;
    }

    auto targetQuat = Utils::lookAtDirection2(dir);

    // if (auto characterControllerr = thisObject_.GetComponent<CharacterController>())
    // {
    //     LOG_DEBUG << "Pus   t: " << targetQuat << ", current state= " << characterControllerr->getCurrentStateName();
    //     characterControllerr->pushEventToQueue(RotateTargetEvent{.target = targetQuat});
    //     return;
    // }

    TweenTransform tween;
    tween.rotation = Rotation(targetQuat);
    componentContext_.tweenManager.Add(thisObject_, tween, 0.5f, EaseType::CubicOut);
}
void DialogueComponent::RestoreRotation()
{
    if (tmpRotation)
    {
        const float duration{0.5f};
        componentContext_.tweenManager.Add(thisObject_, TweenTransform{.rotation = *tmpRotation}, duration, EaseType::CubicOut);
        tmpRotation.reset();
    }
}
const DialogueNode* DialogueComponent::goToNode(int nextNodeID)
{
    LOG_DEBUG << nextNodeID;

    if (nextNodeID < 0)
    {
        LOG_DEBUG << "End dialog";
        currentNodeID = startNodeID;
    }
    else
    {
        auto iter = nodes.find(nextNodeID);
        if (iter != nodes.end())
        {
            currentNodeID = nextNodeID;
            return &iter->second;
        }
        else
        {
            LOG_WARN << "Target not exist : " << nextNodeID;
        }
    }

    return nullptr;
}
void DialogueComponent::setNodes(Nodes&& n)
{
    nodes = std::move(n);
}
void DialogueComponent::resetCurrent()
{
    currentNodeID = startNodeID;
}
}  // namespace Components
}  // namespace GameEngine
