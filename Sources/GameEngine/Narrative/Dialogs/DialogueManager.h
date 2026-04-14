#pragma once
#include <EngineApi.h>

#include <mutex>

#include "DialogueNode.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogContext.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogEvents.h"
#include "GameEngine/Narrative/Dialogs/Fsm/DialogFsm.h"

namespace Utils::Time
{
class ITimerService;
}

namespace Input
{
class InputManager;
}

namespace GameEngine
{
namespace GUI
{
class Manager;
class IElementFactory;
}  // namespace GUI

class ITweenManager;
class IAudioManager;
class GameState;
class GameObject;

namespace Components
{
class DialogueComponent;
}  // namespace Components

class ENGINE_API DialogueManager
{
public:
    DialogueManager(IAudioManager&, Utils::Time::ITimerService&, Input::InputManager&, GUI::IElementFactory&, GUI::Manager&,
                    GameState&, ITweenManager&, std::function<void(EngineEvent)>);

    void startDialogue(GameObject&, Components::DialogueComponent&);
    bool isActive() const;
    void processEvents();
    void handleEvent(const DialogEvent&);

private:
    using EventQueue = std::deque<DialogEvent>;
    EventQueue eventQueue;
    std::mutex eventQueueMutex;

private:
    DialogContext dialogContext;
    DialogFsm dialogFsm;
};
}  // namespace GameEngine
