#include "DialogueManager.h"

#include <Logger/Log.h>
#include <Time/TimerService.h>

#include <utility>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Camera/CameraComponent.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Dialogs/DialogueOption.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/Layout/Layout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Scene/EaseType.h"
#include "GameEngine/Scene/ITweenManager.h"
#include "GameEngine/Scene/Tween.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"

using namespace std::chrono_literals;

namespace GameEngine
{
namespace
{
const uint32 WRAP_WIDTH{1500};
const vec3 HIGHLIGHT_COLOR(1, 1, 0);

const std::chrono::milliseconds calculateTimer(const std::string& text)
{
    const int baseTimeMs  = 1500;
    const int timePerChar = 35;
    const int maxTimeMs   = 8000;

    int calculatedTime = baseTimeMs + (static_cast<int>(text.length()) * timePerChar);

    calculatedTime = std::min(calculatedTime, maxTimeMs);

    LOG_DEBUG << "calculateTimer :  " << calculatedTime;

    return std::chrono::milliseconds(calculatedTime);
}
}  // namespace
DialogueManager::DialogueManager(Utils::Time::ITimerService& timerService, Input::InputManager& inputManager,
                                 IGuiElementFactory& factory, GuiManager& manger, GameState& gs, ITweenManager& tweenManager)
    : timerService(timerService)
    , inputManager(inputManager)
    , guiFactory(factory)
    , guiManager(manger)
    , subscriptions_(inputManager)
    , gameState(gs)
    , tweenManager(tweenManager)
{
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::UARROW,
                                                     [&]()
                                                     {
                                                         if (not dialogueComponent)
                                                             return;

                                                         int oldItem = highlighted;
                                                         highlighted--;
                                                         if (highlighted < 0)
                                                         {
                                                             highlighted = visibleOptions.size() - 1;
                                                         }
                                                         int newItem = highlighted;
                                                         updateHighLightedColor(oldItem, newItem);
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::DARROW,
                                                     [&]()
                                                     {
                                                         if (not dialogueComponent)
                                                             return;

                                                         int oldItem = highlighted;
                                                         highlighted++;
                                                         if (highlighted >= static_cast<int>(visibleOptions.size()))
                                                         {
                                                             highlighted = 0;
                                                         }
                                                         int newItem = highlighted;
                                                         updateHighLightedColor(oldItem, newItem);
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::ENTER,
                                                     [&]()
                                                     {
                                                         if (isActive())
                                                         {
                                                             if (visibleOptions.size() > highlighted)
                                                             {
                                                                 auto [index, _] = visibleOptions[highlighted];
                                                                 selectOption(index);
                                                                 highlighted = 0;
                                                             }
                                                         }
                                                     });
}
void DialogueManager::startDialogue(GameObject& gameObject, Components::DialogueComponent& component)
{
    if (component.GetDialog().empty())
    {
        LOG_DEBUG << "Can not start empty dialog in gameObject : " << component.GetParentGameObject().GetName();
        return;
    }

    visibleOptions.clear();
    dialogueComponent = &component;
    this->npcName     = dialogueComponent->GetParentGameObject().GetName();
    this->gameObject  = &gameObject;

    component.RotateObjectToPlayer(gameObject.GetWorldTransform().GetPosition());

    auto show = [&]()
    {
        LOG_DEBUG << "Init gui and refresh options";
        initGui();
        refreshOptionGui();
    };

    cameraComponent = gameObject.GetComponentInChild<Components::CameraComponent>();
    if (cameraComponent)
    {
        thridPersonCameraComponent = gameObject.GetComponent<Components::ThridPersonCameraComponent>();
        if (thridPersonCameraComponent)
        {
            thridPersonCameraComponent->pushEventToQueue(Components::Camera::StartScriptedMode{});
        }

        const float duration{0.5f};

        if (auto targetTransform = calculateCameraTarget())
        {
            tweenManager.Add(cameraComponent->GetParentGameObject(), *targetTransform, duration, EaseType::CubicOut, show);
        }
        else
        {
            show();
        }
    }
    else
    {
        show();
    }
}
void DialogueManager::selectOption(int optionIndex)
{
    LOG_DEBUG << "selectOption " << optionIndex;

    if (not dialogueComponent)
    {
        return;
    }

    auto status = dialogueComponent->selectOption(optionIndex);
    if (status == Components::DialogueComponent::SelectOptionResult::end)
    {
        EndDialog();
        return;
    }

    refreshOptionGui();
}
void DialogueManager::goToNode(int nodeIndex)
{
    LOG_DEBUG << "nodeIndex " << nodeIndex;

    if (not dialogueComponent)
    {
        return;
    }

    auto status = dialogueComponent->goToNode(nodeIndex);
    if (status == Components::DialogueComponent::SelectOptionResult::end)
    {
        EndDialog();
        return;
    }

    refreshOptionGui();
}
bool DialogueManager::isActive() const
{
    return dialogueComponent != nullptr;
}
void DialogueManager::EndDialog()
{
    LOG_DEBUG << "EndDialog";

    dialogueComponent->RestoreRotation();
    dialogueComponent = nullptr;
    gameObject        = nullptr;
    npcName.clear();

    if (textDialogueWindow)
        textDialogueWindow->Hide();
    if (optionsDialogueWindow)
        optionsDialogueWindow->Hide();

    if (thridPersonCameraComponent)
    {
        thridPersonCameraComponent->pushEventToQueue(Components::Camera::StopScriptedMode{});
        thridPersonCameraComponent = nullptr;
    }

    if (cameraComponent)
    {
        tweenManager.Remove(cameraComponent->GetParentGameObject());
        cameraComponent = nullptr;
    }
}
void DialogueManager::initGui()
{
    auto createWindowWithLayout = [&](const vec2& position, const vec2& scale)
    {
        auto dialogueWindowPtr  = guiFactory.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, position, scale);
        auto textDialogueWindow = dialogueWindowPtr.get();
        auto layout             = guiFactory.CreateVerticalLayout();
        auto textWindowLayout   = layout.get();
        dialogueWindowPtr->AddChild(std::move(layout));
        guiManager.Add(std::move(dialogueWindowPtr));

        return std::make_pair(textDialogueWindow, textWindowLayout);
    };
    if (not textDialogueWindow)
    {
        const vec2& position{0.5f, 0.75f};
        const vec2& scale{0.5f, 0.2f};
        auto [window, layout] = createWindowWithLayout(position, scale);
        textDialogueWindow    = window;
        textWindowLayout      = layout;
    }
    else
    {
        textDialogueWindow->Show();
    }

    if (not optionsDialogueWindow)
    {
        const vec2& position{0.5f, 0.25f};
        const vec2& scale{0.95f, 0.24f};
        auto [window, layout] = createWindowWithLayout(position, scale);
        layout->SetAlgin(Layout::Algin::LEFT);
        optionsDialogueWindow = window;
        optionsWindowLayout   = layout;
        hideOptions();
    }
    else
    {
        hideOptions();
    }
}
void DialogueManager::updateHighLightedColor(int oldItem, int newItem)
{
    auto& children = optionsWindowLayout->GetChildren();
    if (not optionsWindowLayout or children.empty())
        return;

    auto updateColor = [&](int index, glm::vec3 color)
    {
        if (index >= 0 && index < static_cast<int>(children.size()))
        {
            if (auto* text = dynamic_cast<GuiTextElement*>(children[index].get()))
            {
                text->SetColor(color);
            }
        }
    };

    updateColor(oldItem, vec3(1, 1, 1));
    updateColor(newItem, HIGHLIGHT_COLOR);
}

std::vector<std::pair<int, DialogueOption>> DialogueManager::getVisibleOptions(const DialogueNode& node) const
{
    std::vector<std::pair<int, DialogueOption>> visibleOptions;

    int index = 0;
    for (const auto& option : node.options)
    {
        bool isAvailable = true;

        for (const auto& condition : option.conditions)
        {
            bool hasFlag = gameState.hasFlag(condition.flag);

            if (condition.type == ConditionType::REQUIRED and not hasFlag)
            {
                isAvailable = false;
                break;
            }
            if (condition.type == ConditionType::FORBIDDEN and hasFlag)
            {
                isAvailable = false;
                break;
            }
        }

        if (isAvailable)
        {
            visibleOptions.push_back({index, option});
        }

        ++index;
    }

    return visibleOptions;
}

void DialogueManager::refreshOptionGui()
{
    if (not dialogueComponent)
        return;

    if (textWindowLayout)
        textWindowLayout->RemoveAll();
    if (optionsWindowLayout)
    {
        optionsWindowLayout->RemoveAll();
        hideOptions();
    }

    if (auto current = dialogueComponent->getCurrent())
    {
        const vec2 textSize{1.0f, 0.25};
        auto npcNameGuiText = guiFactory.CreateGuiTextWrapped(npcName, 0);
        npcNameGuiText->SetLocalScale(textSize + vec2(0.1f, 0.1f));
        textWindowLayout->AddChild(std::move(npcNameGuiText));

        auto npcGuiText = guiFactory.CreateGuiTextWrapped(current->npcText, WRAP_WIDTH);
        npcGuiText->SetLocalScale({1.0f, 0.4});
        textWindowLayout->AddChild(std::move(npcGuiText));

        if (not current->setGameStateflag.empty())
        {
            gameState.setFlag(current->setGameStateflag, true);
        }
        if (not current->removeGameStateFlag.empty())
        {
            gameState.setFlag(current->removeGameStateFlag, false);
        }

        timerService.timer(calculateTimer(current->npcText),
                           [this, textSize]()
                           {
                               LOG_DEBUG << "Text timer expired";
                               if (not dialogueComponent)
                               {
                                   LOG_WARN << "Dialog comopnent not set!?";
                                   return;
                               }

                               if (auto current = dialogueComponent->getCurrent())
                               {
                                   visibleOptions = getVisibleOptions(*current);
                                   if (visibleOptions.empty())
                                   {
                                       if (current->nextNodeID != INVALID_NODE_ID)
                                       {
                                           goToNode(current->nextNodeID);
                                       }
                                       else
                                       {
                                           EndDialog();
                                       }
                                       return;
                                   }

                                   showOptions();
                                   int i = 0;
                                   for (const auto& [_, option] : visibleOptions)
                                   {
                                       auto optionGuiText = guiFactory.CreateGuiTextWrapped(option.text, WRAP_WIDTH);
                                       optionGuiText->SetLocalScale(textSize);
                                       optionGuiText->SetAlgin(GuiTextElement::Algin::LEFT);
                                       if (i == 0)
                                       {
                                           optionGuiText->SetColor(HIGHLIGHT_COLOR);
                                           i++;
                                       }
                                       optionsWindowLayout->AddChild(std::move(optionGuiText));
                                   }
                               }
                           });
    }
}
std::optional<TweenTransform> DialogueManager::calculateCameraTarget()
{
    vec3 playerPos = gameObject->GetWorldTransform().GetPosition();
    vec3 npcPos    = dialogueComponent->GetParentGameObject().GetWorldTransform().GetPosition();

    vec3 diff           = npcPos - playerPos;
    float distanceSq    = dot(diff, diff);
    const float epsilon = 0.0001f;
    if (distanceSq < epsilon)
    {
        LOG_WARN << "Player and NPC are in the same position! Returning default camera transform.";
        return {};
    }

    vec3 forward = normalize(diff);
    vec3 up      = (std::abs(forward.y) > 0.999f) ? vec3(0, 0, 1) : vec3(0, 1, 0);
    vec3 right   = normalize(cross(up, forward));

    vec3 targetPos   = playerPos - (forward * 0.6f) + (right * 1.6f) + vec3(0, 1.7f, 0);
    vec3 lookAtPoint = npcPos + vec3(0, 1.6f, 0);
    auto targetRot   = Utils::lookAt(lookAtPoint, targetPos);

    TweenTransform target;
    target.position = targetPos;
    target.rotation = targetRot;

    return target;
}
void DialogueManager::hideOptions()
{
    optionsDialogueWindow->Hide();
    for (auto& child : optionsDialogueWindow->GetChildren())
    {
        child->Hide();
    }
}
void DialogueManager::showOptions()
{
    optionsDialogueWindow->Show();
    for (auto& child : optionsDialogueWindow->GetChildren())
    {
        child->Show();
    }
}
}  // namespace GameEngine
