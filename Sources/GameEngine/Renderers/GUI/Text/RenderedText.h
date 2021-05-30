#pragma once
#include <Types.h>
#include "Surface.h"
#include <Utils/Image/Image.h>

namespace GameEngine
{
struct RenderedText
{
    uint32 fontId;
    std::string text;
    Color color;
    uint32 outline;
    Utils::Image image;
    uint32 insances;
};
}  // namespace GameEngine
