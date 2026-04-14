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
namespace GUI
{
class Manager;
class Window;
class IElementFactory;
class VerticalLayout;
}  // namespace GUI

class IAudioManager;
class GameState;
class ITweenManager;
class Scene;

struct DialogContext
{
    Utils::Time::ITimerService& timerService;
    IAudioManager& audioManager;
    Input::InputManager& inputManager;
    GUI::IElementFactory& guiFactory;
    GUI::Manager& guiManager;
    GameState& gameState;
    ITweenManager& tweenManager;
    std::function<void(DialogEvent&&)> sendEvent;
    std::function<void(EngineEvent)> addEngineEvent;

    struct GuiWindow
    {
        GUI::Window* window{nullptr};
        GUI::VerticalLayout* layout{nullptr};
    };

    GuiWindow sentenceWindow;
    GuiWindow optionsWindow;
};
void setAnimation(GameObject&, StartDialogEvent::Role);
}  // namespace GameEngine
