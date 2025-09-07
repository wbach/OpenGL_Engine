#pragma once
#include <Types.h>

namespace GameEngine
{
struct GuiTheme
{
    uint32 fontOutline              = 0;
    uint32 fontSize_                = 32;
    std::string font                = "segoe-ui.ttf";
    std::string editBoxBackground   = "whiteClear.png";
    std::string backgroundTexture   = "darkGrayButton.png";
    std::string buttonTexture       = "darkGrayButton.png";
    std::string buttonHoverTexture  = "darkGrayButtonHover.png";
    std::string buttonActiveTexture = "darkGrayButtonActive.png";
    std::string windowBarTexture    = "darkGrayButton.png";
    Color backgroundColor;
    Color editBoxBackgroundColor;
    Color buttonBackgroundColor;
    Color buttonHoverColor;
    Color buttonActiveColor;
};
}  // namespace GameEngine
