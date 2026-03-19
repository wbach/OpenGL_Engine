#pragma once
#include <functional>

#include "DialogEvents.h"

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
struct GameState;
class ITweenManager;
class GuiWindowElement;
class VerticalLayout;

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

    struct GuiWindow
    {
        GuiWindowElement* window{nullptr};
        VerticalLayout* layout{nullptr};
    };

    GuiWindow sentenceWindow;
    GuiWindow optionsWindow;
};
}  // namespace GameEngine
