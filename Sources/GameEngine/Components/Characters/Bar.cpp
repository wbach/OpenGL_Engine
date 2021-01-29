#include "Bar.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

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
        texture->SetLocalScale(vec2(p, texture->GetLocalScale().y));
    }
}
}  // namespace Components
}  // namespace GameEngine
