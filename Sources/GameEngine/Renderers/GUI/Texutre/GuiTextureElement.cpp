#include "GuiTextureElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace
{
const std::string emptyString = "";
} // namespace

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
const std::string& GuiTextureElement::GetFilename() const
{
    return texture_ ? texture_->GetFileName() : emptyString;
}
}  // namespace GameEngine
