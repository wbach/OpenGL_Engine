#pragma once
#include <string>
#include "Types.h"

namespace GameEngine
{
struct GuiTextElement
{
    std::string text = "default text";
    vec2 position;
    vec3 colour;
    float m_size  = 1;
    bool isActive = true;
};
} // namespace GameEngine
