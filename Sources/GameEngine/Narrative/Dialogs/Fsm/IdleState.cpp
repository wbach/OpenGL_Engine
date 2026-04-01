#include "IdleState.h"

#include "DialogContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/IGuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"

namespace GameEngine
{
namespace
{
void sendEventToCharactercontroller(GameObject& gameObject, CharacterControllerEvent event)
{
    if (auto characterController = gameObject.GetComponent<Components::CharacterController>())
    {
        characterController->pushEventToQueue(event);
    }
};
}  // namespace

IdleState::IdleState(DialogContext& context)
    : dialogContext{context}
{
}
void IdleState::initGui()
{
    auto createWindowWithLayout = [&](const vec2& position, const vec2& scale)
    {
        auto dialogueWindowPtr  = dialogContext.guiFactory.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, position, scale);
        auto textDialogueWindow = dialogueWindowPtr.get();
        auto layout             = dialogContext.guiFactory.CreateVerticalLayout();
        layout->AutoHideElements(false);
        auto textWindowLayout   = layout.get();
        dialogueWindowPtr->AddChild(std::move(layout));
        dialogContext.guiManager.Add(std::move(dialogueWindowPtr));

        return DialogContext::GuiWindow{.window = textDialogueWindow, .layout = textWindowLayout};
    };

    if (not dialogContext.sentenceWindow.window or not dialogContext.sentenceWindow.layout)
    {
        const vec2& position{0.5f, 0.75f};
        const uint32 WRAP_WIDTH{EngineConf.window.size->x - 20};
        const vec2& scale{(float)WRAP_WIDTH / (float)EngineConf.window.size->x, 0.25f};
        dialogContext.sentenceWindow = createWindowWithLayout(position, scale);
        dialogContext.sentenceWindow.window->Hide();
    }

    if (not dialogContext.optionsWindow.window or not dialogContext.optionsWindow.layout)
    {
        const vec2& position{0.5f, 0.25f};
        const vec2& scale{0.95f, 0.24f};
        dialogContext.optionsWindow = createWindowWithLayout(position, scale);
        dialogContext.optionsWindow.layout->SetAlgin(Layout::Algin::LEFT);
        dialogContext.optionsWindow.window->Hide();
    }
}
void IdleState::onEnter()
{
    auto hideAndClear = [](DialogContext::GuiWindow& window)
    {
        if (window.window)
        {
            window.window->Hide();
        }

        if (window.layout)
        {
            window.layout->RemoveAll();
        }
    };

    hideAndClear(dialogContext.sentenceWindow);
    hideAndClear(dialogContext.optionsWindow);
}
void IdleState::onEnter(const EndDialog& event)
{
    dialogContext.inputManager.StashPopSubscribers();

    sendEventToCharactercontroller(event.playerGameObject, EndDialogEvent{});
    sendEventToCharactercontroller(event.component.GetParentGameObject(), EndDialogEvent{});
    dialogContext.addEngineEvent(SetGameStateFlag{.flag = event.component.GetName() + "_end", .value = 1});
}

void IdleState::onLeave(const StartRequested& event)
{
    sendEventToCharactercontroller(event.playerGameObject, StartDialogEvent{.role = StartDialogEvent::Role::Waiting});
    sendEventToCharactercontroller(event.component.GetParentGameObject(),
                                   StartDialogEvent{.role = StartDialogEvent::Role::Waiting});

    initGui();
    dialogContext.inputManager.StashSubscribers();
}
}  // namespace GameEngine
