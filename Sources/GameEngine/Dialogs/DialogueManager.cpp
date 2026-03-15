#include "DialogueManager.h"

#include <Logger/Log.h>

#include <utility>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Camera/CameraComponent.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/Layout/Layout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Scene/EaseType.h"
#include "GameEngine/Scene/Tween.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
namespace
{
const vec3 highlightedColor(1, 1, 0);
}
DialogueManager::DialogueManager(Input::InputManager& inputManager, GuiElementFactory& factory, GuiManager& manger, GameState& gs,
                                 TweenManager& tweenManager)
    : inputManager(inputManager)
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

                                                         if (auto current = dialogueComponent->getCurrent())
                                                         {
                                                             int oldItem = highlighted;
                                                             highlighted--;
                                                             if (highlighted < 0)
                                                             {
                                                                 highlighted = current->options.size() - 1;
                                                             }
                                                             int newItem = highlighted;
                                                             updateHighLightedColor(oldItem, newItem);
                                                         }
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::DARROW,
                                                     [&]()
                                                     {
                                                         if (not dialogueComponent)
                                                             return;

                                                         if (auto current = dialogueComponent->getCurrent())
                                                         {
                                                             int oldItem = highlighted;
                                                             highlighted++;
                                                             if (highlighted >= static_cast<int>(current->options.size()))
                                                             {
                                                                 highlighted = 0;
                                                             }
                                                             int newItem = highlighted;
                                                             updateHighLightedColor(oldItem, newItem);
                                                         }
                                                     });
    subscriptions_ = inputManager.SubscribeOnKeyDown(KeyCodes::ENTER,
                                                     [&]()
                                                     {
                                                         if (isActive())
                                                         {
                                                             selectOption(highlighted);
                                                             highlighted = 0;
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
        tweenManager.Add(cameraComponent->GetParentGameObject(), calculateCameraTarget(), duration, EaseType::CubicOut, show);
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
bool DialogueManager::isActive() const
{
    return dialogueComponent != nullptr;
}
void DialogueManager::EndDialog()
{
    LOG_DEBUG << "";

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
    }
    else
    {
        optionsDialogueWindow->Show();
    }
}
void DialogueManager::updateHighLightedColor(int oldItem, int newItem)
{
    auto& children = optionsWindowLayout->GetChildren();
    if (!optionsWindowLayout || children.empty())
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
    updateColor(newItem, highlightedColor);
}
void DialogueManager::refreshOptionGui()
{
    if (not dialogueComponent)
        return;

    textWindowLayout->RemoveAll();
    optionsWindowLayout->RemoveAll();

    if (auto current = dialogueComponent->getCurrent())
    {
        const vec2 textSize{1.0f, 0.25};
        auto npcNameGuiText = guiFactory.CreateGuiText(npcName);
        npcNameGuiText->SetLocalScale(textSize + vec2(0.1f, 0.1f));
        textWindowLayout->AddChild(std::move(npcNameGuiText));

        auto npcGuiText = guiFactory.CreateGuiText(current->npcText);
        npcGuiText->SetLocalScale(textSize);
        textWindowLayout->AddChild(std::move(npcGuiText));

        int i = 0;
        for (const auto& option : current->options)
        {
            auto optionGuiText = guiFactory.CreateGuiText(option.text);
            optionGuiText->SetLocalScale(textSize);
            optionGuiText->SetAlgin(GuiTextElement::Algin::LEFT);
            if (i == 0)
            {
                optionGuiText->SetColor(highlightedColor);
                i++;
            }
            optionsWindowLayout->AddChild(std::move(optionGuiText));
        }
    }
}
TweenTransform DialogueManager::calculateCameraTarget()
{
    vec3 playerPos = gameObject->GetWorldTransform().GetPosition();
    vec3 npcPos    = dialogueComponent->GetParentGameObject().GetWorldTransform().GetPosition();

    vec3 forward = normalize(npcPos - playerPos);
    vec3 right   = cross(vec3(0, 1, 0), forward);

    vec3 targetPos = playerPos - (forward * 0.6f) + (right * 1.6f) + vec3(0, 1.7f, 0);

    vec3 lookAtPoint = npcPos + vec3(0, 1.6f, 0);
    auto targetRot   = Utils::lookAt(lookAtPoint, targetPos);

    TweenTransform target;
    target.position = targetPos;
    target.rotation = targetRot;

    return target;
}
}  // namespace GameEngine
