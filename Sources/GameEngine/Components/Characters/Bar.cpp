#include "Bar.h"
#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"

namespace GameEngine
{
namespace Components
{
void Bar::update()
{
    if (texture and (currentRendered != current or maxRendered != maxValue))
    {
        maxRendered     = maxValue;
        currentRendered = current;

        auto p = static_cast<float>(current) / static_cast<float>(maxValue);
        texture->setLocalScale(vec2(p, texture->getLocalScale().y));
    }
}
}  // namespace Components
}  // namespace GameEngine
