#include "GuiTextureElement.h"

namespace GameEngine
{
GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(const vec2ui& windowSize, const Texture& texture)
    : GuiElement(type, windowSize)
    , texture_(texture)
{
}
const Texture& GuiTextureElement::GetTexture() const
{
    return texture_;
}
void GuiTextureElement::Flip()
{
    scale_.y *= -1;
    CalculateMatrix();
}
}  // namespace GameEngine
