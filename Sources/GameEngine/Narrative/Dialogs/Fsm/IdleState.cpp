#include "IdleState.h"

#include "DialogContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineEvent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/ElementReader.h"
#include "GameEngine/Renderers/GUI/ElementWriter.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/IElementVisitor.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "Logger/Log.h"
#include "TreeNode.h"
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
    if (isInit)
        return;

    const std::string layerName{"Dialog"};
    GUI::ElementReader reader(dialogContext.guiManager, dialogContext.guiFactory);

    const auto& dialogFilePath = EngineLocalConf.files.getDialogGuiPath();
    reader.read(dialogFilePath, layerName, {});

    if (auto layer = dialogContext.guiManager.getLayer(layerName))
    {
        dialogContext.sentenceWindow = layer->getTypedElement<GUI::Window>("SentenceWindow");
        dialogContext.npcNameText    = layer->getTypedElement<GUI::Text>("SentenceName");
        dialogContext.sentenceText   = layer->getTypedElement<GUI::MultiLineText>("Sentence");
        dialogContext.optionsWindow  = layer->getTypedElement<GUI::Window>("OptionsWindow");
        dialogContext.optionsLayout  = layer->getTypedElement<GUI::VerticalLayout>("OptionsLayout");

        if (dialogContext.sentenceWindow)
        {
            dialogContext.sentenceWindow->activate(false);
        }
        if (dialogContext.optionsWindow)
        {
            dialogContext.optionsWindow->activate(false);
        }

        if (dialogContext.optionsLayout)
        {
            const auto& children = dialogContext.optionsLayout->getChildren();
            if (not children.empty())
            {
                if (auto text = dynamic_cast<GUI::Text*>(children.front().get()))
                {
                    GUI::ElementWriter::write(dialogContext.optionsButtonTemplate, *text);
                }
                else
                {
                    LOG_WARN << "Wrong element type";
                }
            }
            else
            {
                LOG_WARN << "No option template text";
            }

            dialogContext.optionsLayout->removeAll();
        }
    }

    isInit = true;
}
void IdleState::onEnter()
{
    if (dialogContext.sentenceWindow)
    {
        dialogContext.sentenceWindow->activate(false);
    }

    if (dialogContext.optionsWindow)
    {
        dialogContext.optionsWindow->activate(false);
    }
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
