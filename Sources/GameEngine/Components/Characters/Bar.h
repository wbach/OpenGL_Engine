#pragma once
#include <Types.h>

namespace GameEngine
{
class GuiTextureElement;

namespace Components
{
struct Bar
{
    int64& current;
    int64& maxValue;

    GuiTextureElement* texture{nullptr};
    int64 maxRendered{0};
    int64 currentRendered{0};

    void update();
};
}  // namespace Components
}  // namespace GameEngine
