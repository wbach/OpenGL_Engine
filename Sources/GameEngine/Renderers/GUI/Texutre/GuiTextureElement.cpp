#include "GuiTextureElement.h"

namespace GameEngine
{
GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, const vec2ui& windowSize, Texture& texture)
    : GuiRendererElementBase(renderSubscribe, unsubscribeElement, GuiTextureElement::type, windowSize)
{
    texture_ = &texture;
}

void GuiTextureElement::Flip()
{
    scale_.y *= -1;
    CalculateMatrix();
}
}  // namespace GameEngine
