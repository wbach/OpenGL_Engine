#include "PauseMenuTheme.h"

namespace AvatarGame
{
GameEngine::GuiTheme getGuiTheme()
{
    GameEngine::GuiTheme guiTheme;
    guiTheme.font = "GUI/herculanum.ttf";
    guiTheme.fontSize_ = 50;
    guiTheme.backgroundColor.value.w = 0.5f;
    guiTheme.buttonBackgroundColor.value.w = 0.f;
    guiTheme.buttonHoverTexture = guiTheme.backgroundTexture;
    return guiTheme;
}
}  // namespace AvatarGame
