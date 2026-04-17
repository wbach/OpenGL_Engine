#include "WaitingForInput.h"

#include <Input/InputManager.h>
#include <Input/KeyCodes.h>
#include <Logger/Log.h>

#include "DialogContext.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h"
#include "GameEngine/Renderers/GUI/ElementReader.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"

namespace GameEngine
{
namespace
{
const vec3 HIGHLIGHT_COLOR(1, 1, 0);
}

WaitingForInput::WaitingForInput(DialogContext& dialogContext)
    : dialogContext{dialogContext}
    , subscriptions_(dialogContext.inputManager)
{
}
void WaitingForInput::onEnter()
{
    if (dialogContext.optionsWindow)
        dialogContext.optionsWindow->activate(true);
    if (dialogContext.optionsLayout)
        dialogContext.optionsLayout->removeAll();
    subscribeForInput();
}
void WaitingForInput::onEnter(const StartInputWaiting& event)
{
    setAnimations(event.playerGameObject, event.component.GetParentGameObject());

    int i            = 0;
    visibleOptions   = event.visibleOptions;
    component        = &event.component;
    playerGameObject = &event.playerGameObject;
    LOG_DEBUG << "visibleOptions : " << visibleOptions;

    for (const auto& [_, option] : visibleOptions)
    {
        GUI::ElementReader reader(dialogContext.guiManager, dialogContext.guiFactory);
        auto optionGuiText       = reader.readText(dialogContext.optionsButtonTemplate);
        optionGuiText->text.text = option.text;

        if (i == 0)
        {
            optionGuiText->setColor(HIGHLIGHT_COLOR);
            i++;
        }

        if (dialogContext.optionsLayout)
            dialogContext.optionsLayout->addChild(std::move(optionGuiText));
    }
}
void WaitingForInput::onEnter(const BackToSentence& event)
{
    onEnter(StartInputWaiting{
        .visibleOptions = event.visibleOptions, .playerGameObject = event.playerGameObject, .component = event.component});
}
void WaitingForInput::subscribeForInput()
{
    subscriptions_ = dialogContext.inputManager.SubscribeOnKeyDown(KeyCodes::UARROW,
                                                                   [this]()
                                                                   {
                                                                       auto oldItem = highlighted;
                                                                       highlighted--;
                                                                       if (highlighted < 0)
                                                                       {
                                                                           highlighted = visibleOptions.size() - 1;
                                                                       }
                                                                       auto newItem = highlighted;
                                                                       updateHighLightedColor(oldItem, newItem);
                                                                   });
    subscriptions_ = dialogContext.inputManager.SubscribeOnKeyDown(KeyCodes::DARROW,
                                                                   [this]()
                                                                   {
                                                                       auto oldItem = highlighted;
                                                                       highlighted++;
                                                                       if (highlighted >= visibleOptions.size())
                                                                       {
                                                                           highlighted = 0;
                                                                       }
                                                                       auto newItem = highlighted;
                                                                       updateHighLightedColor(oldItem, newItem);
                                                                   });
    subscriptions_ = dialogContext.inputManager.SubscribeOnKeyDown(KeyCodes::ENTER,
                                                                   [this]()
                                                                   {
                                                                       if (visibleOptions.size() > highlighted)
                                                                       {
                                                                           auto [index, _] = visibleOptions[highlighted];
                                                                           highlighted     = 0;
                                                                           selectOption(index);
                                                                       }
                                                                   });
}
void WaitingForInput::onLeave()
{
    subscriptions_.UnsubscribeKeys();

    visibleOptions.clear();
    component   = nullptr;
    highlighted = 0;

    if (dialogContext.optionsWindow)
        dialogContext.optionsWindow->activate(false);
}
void WaitingForInput::updateHighLightedColor(size_t oldItem, size_t newItem)
{
    if (not dialogContext.optionsWindow)
        return;

    auto& children = dialogContext.optionsLayout->getChildren();
    if (children.empty())
        return;

    auto updateColor = [this](auto index, const auto& color)
    {
        auto& children = dialogContext.optionsLayout->getChildren();
        if (index >= 0 && index < children.size())
        {
            if (auto* text = dynamic_cast<GUI::Text*>(children[index].get()))
            {
                text->setColor(color);
            }
        }
    };

    updateColor(oldItem, vec3(1, 1, 1));
    updateColor(newItem, HIGHLIGHT_COLOR);
}
void WaitingForInput::selectOption(size_t optionIndex)
{
    if (not component)
    {
        LOG_WARN << "Something goes wrong. Component not set";
        return;
    }

    if (auto selectedOption = component->selectOption(optionIndex))
    {
        dialogContext.sendEvent(
            OptionSelected{.option = *selectedOption, .playerGameObject = *playerGameObject, .component = *component});
        return;
    }

    dialogContext.sendEvent(EndDialog{.playerGameObject = *playerGameObject, .component = *component});
}
void WaitingForInput::setAnimations(GameObject& listener, GameObject& speaker)
{
    setAnimation(listener, StartDialogEvent::Role::Waiting);
    setAnimation(speaker, StartDialogEvent::Role::Waiting);
}
}  // namespace GameEngine
