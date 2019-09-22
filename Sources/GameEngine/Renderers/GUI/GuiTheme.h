#pragma once
#include <Types.h>

namespace GameEngine
{
struct GuiTheme
{
    uint32 fontOutline              = 0;
    uint32 fontSize_                = 32;
    std::string font                = "GUI/Ubuntu-M.ttf";
    std::string editBoxBackground   = "GUI/whiteClear.png";
    std::string backgroundTexture   = "GUI/gray.png";
    std::string buttonTexture       = "GUI/darkGrayButton.png";
    std::string buttonHoverTexture  = "GUI/darkGrayButtonHover.png";
    std::string buttonActiveTexture = "GUI/darkGrayButtonActive.png";
    std::string windowBarTexture    = "GUI/darkGrayButton.png";
};
}  // namespace GameEngine
