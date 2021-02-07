#pragma once
#include <Types.h>

namespace GameEngine
{
struct GuiTheme
{
    uint32 fontOutline              = 0;
    uint32 fontSize_                = 32;
    std::string font                = "GUI/segoe-ui.ttf";
    std::string editBoxBackground   = "GUI/whiteClear.png";
    std::string backgroundTexture   = "GUI/darkGrayButton.png";
    std::string buttonTexture       = "GUI/darkGrayButton.png";
    std::string buttonHoverTexture  = "GUI/darkGrayButtonHover.png";
    std::string buttonActiveTexture = "GUI/darkGrayButtonActive.png";
    std::string windowBarTexture    = "GUI/darkGrayButton.png";
    Color backgroundColor;
    Color editBoxBackgroundColor;
    Color buttonBackgroundColor;
    Color buttonHoverColor;
    Color buttonActiveColor;
};
}  // namespace GameEngine
