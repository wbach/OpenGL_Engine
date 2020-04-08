#include "GuiTextureElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace
{
const std::string emptyString = "";
} // namespace

GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(GUIRenderer& guiRenderer, const vec2ui& windowSize, Texture& texture)
    : GuiRendererElementBase(guiRenderer, GuiTextureElement::type, windowSize)
{
    texture_ = &texture;
}

void GuiTextureElement::Flip()
{
    scale_.y *= -1;
}
const std::string& GuiTextureElement::GetFilename() const
{
    return texture_ ? texture_->GetFileName() : emptyString;
}
}  // namespace GameEngine
