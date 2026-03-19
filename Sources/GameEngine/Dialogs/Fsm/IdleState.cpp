#include "IdleState.h"

#include "DialogContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/IGuiElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"

namespace GameEngine
{
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
        auto textWindowLayout   = layout.get();
        dialogueWindowPtr->AddChild(std::move(layout));
        dialogContext.guiManager.Add(std::move(dialogueWindowPtr));

        return DialogContext::GuiWindow{.window = textDialogueWindow, .layout = textWindowLayout};
    };

    if (not dialogContext.sentenceWindow.window or not dialogContext.sentenceWindow.layout)
    {
        const vec2& position{0.5f, 0.75f};
        const vec2& scale{0.5f, 0.2f};
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
void IdleState::onEnter(const EndDialog&)
{
    if (stashedInputSubscribers)
    {
        dialogContext.inputManager.StashPopSubscribers();
        stashedInputSubscribers = false;
    }
}
void IdleState::onLeave()
{
    initGui();

    dialogContext.inputManager.StashSubscribers();
    stashedInputSubscribers = true;
}
}  // namespace GameEngine
