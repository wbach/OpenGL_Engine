#pragma once
#include <functional>

#include "DialogEvents.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Scene/Scene.hpp"

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
class IAudioManager;
class IGuiElementFactory;
class GuiManager;
class GameState;
class ITweenManager;
class GuiWindowElement;
class VerticalLayout;
class Scene;

const uint32_t WRAP_WIDTH{1500};

struct DialogContext
{
    Utils::Time::ITimerService& timerService;
    IAudioManager& audioManager;
    Input::InputManager& inputManager;
    IGuiElementFactory& guiFactory;
    GuiManager& guiManager;
    GameState& gameState;
    ITweenManager& tweenManager;
    std::function<void(DialogEvent&&)> sendEvent;
    std::function<void(EngineEvent)> addEngineEvent;

    struct GuiWindow
    {
        GuiWindowElement* window{nullptr};
        VerticalLayout* layout{nullptr};
    };

    GuiWindow sentenceWindow;
    GuiWindow optionsWindow;
};
void setAnimation(GameObject&, StartDialogEvent::Role);
}  // namespace GameEngine
