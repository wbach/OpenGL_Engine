#include "GuiWindowStyle.h"

namespace GameEngine
{
std::string convert(GuiWindowStyle style)
{
    switch (style)
    {
        case GuiWindowStyle::BACKGROUND_ONLY:
            return "BACKGROUND_ONLY";
        case GuiWindowStyle::CLOSE:
            return "CLOSE";
        case GuiWindowStyle::EMPTY:
            return "EMPTY";
        case GuiWindowStyle::FULL:
            return "FULL";
        case GuiWindowStyle::WITHOUT_BUTTONS:
            return "WITHOUT_BUTTONS";
    }
    return "FULL";
}
GuiWindowStyle convert(const std::string& str)
{
    if (str == "BACKGROUND_ONLY")
        return GuiWindowStyle::BACKGROUND_ONLY;
    if (str == "CLOSE")
        return GuiWindowStyle::CLOSE;
    if (str == "EMPTY")
        return GuiWindowStyle::EMPTY;
    if (str == "FULL")
        return GuiWindowStyle::FULL;
    if (str == "WITHOUT_BUTTONS")
        return GuiWindowStyle::WITHOUT_BUTTONS;
    return GuiWindowStyle::FULL;
}
}  // namespace GameEngine
