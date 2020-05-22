#pragma once
#include <Types.h>
#include "Surface.h"
#include <GraphicsApi/Image.h>

namespace GameEngine
{
struct RenderedText
{
    uint32 fontId;
    std::string text;
    Color color;
    uint32 outline;
    GraphicsApi::Image image;
    uint32 insances;
};
}  // namespace GameEngine
