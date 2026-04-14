#pragma once
#include <Types.h>

namespace GameEngine
{
namespace GUI
{
enum class FontStyle : uint32
{
    Normal        = 0x00,
    Bold          = 0x01,
    Italic        = 0x02,
    Underline     = 0x04,
    Strikethrough = 0x08
};
}  // namespace GUI
}  // namespace GameEngine
