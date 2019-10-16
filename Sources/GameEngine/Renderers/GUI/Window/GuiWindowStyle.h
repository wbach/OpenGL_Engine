#pragma once
#include <string>

namespace GameEngine
{
enum class GuiWindowStyle
{
    FULL,
    CLOSE,
    WITHOUT_BUTTONS,
    BACKGROUND_ONLY,
    EMPTY
};
GuiWindowStyle convert(const std::string&);
std::string convert(GuiWindowStyle);
}  // namespace GameEngine
