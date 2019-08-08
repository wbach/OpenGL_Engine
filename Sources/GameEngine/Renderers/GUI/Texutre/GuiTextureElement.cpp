#include "GuiTextureElement.h"

namespace GameEngine
{
GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(const vec2ui& windowSize, Texture& texture)
    : GuiRendererElementBase(GuiTextureElement::type, windowSize)
{
    texture_ = &texture;
}

void GuiTextureElement::Flip()
{
    scale_.y *= -1;
    CalculateMatrix();
}
}  // namespace GameEngine
