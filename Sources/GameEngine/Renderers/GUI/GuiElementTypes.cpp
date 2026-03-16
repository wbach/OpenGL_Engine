#include "GuiElementTypes.h"

#include <magic_enum/magic_enum.hpp>

namespace std
{
std::string to_string(GameEngine::GuiElementTypes type)
{
    return std::string(magic_enum::enum_name(type));
}
// clang-format on
}  // namespace std
