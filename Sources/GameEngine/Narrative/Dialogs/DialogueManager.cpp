#include "DialogueManager.h"

#include <Logger/Log.h>
#include <Time/TimerService.h>

#include <filesystem>
#include <mutex>
#include <utility>

#include "GLM/GLMUtils.h"
#include "GameEngine/Audio/IAudioManager.h"
#include "GameEngine/Components/Camera/CameraComponent.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Dialogue/DialogueComponent.h"
#include "GameEngine/Narrative/Dialogs/DialogueOption.h"
#include "GameEngine/Narrative/Dialogs/Fsm/IdleState.h"
#include "GameEngine/Narrative/Dialogs/Fsm/TransitionState.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/Layout/Layout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Text/GuiTextElement.h"
#include "GameEngine/Scene/EaseType.h"
#include "GameEngine/Scene/ITweenManager.h"
#include "GameEngine/Scene/Tween.h"
#include "Input/InputManager.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
DialogueManager::DialogueManager(IAudioManager& audioManager, Utils::Time::ITimerService& timerService,
                                 Input::InputManager& inputManager, IGuiElementFactory& factory, GuiManager& manager,
                                 GameState& gs, ITweenManager& tweenManager)
    : dialogContext{.timerService = timerService,
                    .audioManager = audioManager,
                    .inputManager = inputManager,
                    .guiFactory   = factory,
                    .guiManager   = manager,
                    .gameState    = gs,
                    .tweenManager = tweenManager,
                    .sendEvent =
                        [this](DialogEvent&& event)
                    {
                        std::lock_guard lk(eventQueueMutex);
                        eventQueue.push_back(std::move(event));
                    }}
    , dialogFsm{IdleState{dialogContext}, TransitionState{dialogContext}, ShowingSentence{dialogContext},
                WaitingForInput{dialogContext}}
{
}
void DialogueManager::startDialogue(GameObject& gameObject, Components::DialogueComponent& component)
{
    if (component.GetDialog().empty())
    {
        LOG_DEBUG << "Can not start empty dialog in gameObject : " << component.GetParentGameObject().GetName();
        return;
    }

    LOG_DEBUG << "StartRequested";
    dialogFsm.handle(StartRequested{.playerGameObject = gameObject, .component = component});
}
bool DialogueManager::isActive() const
{
    return not dialogFsm.isCurrentStateOfType<IdleState>();
}
void DialogueManager::processEvents()
{
    EventQueue tmpEventsQueue;

    {
        std::lock_guard<std::mutex> lk(eventQueueMutex);
        tmpEventsQueue = std::move(eventQueue);
    }

    for (auto& event : tmpEventsQueue)
    {
        handleEvent(event);
    }
}
void DialogueManager::handleEvent(const DialogEvent& event)
{
    auto passEventToMachine = [&](const auto& event) { dialogFsm.handle(event); };
    std::visit(passEventToMachine, event);
}
}  // namespace GameEngine
