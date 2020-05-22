#include "GuiTextureElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace
{
const std::string emptyString = "";
} // namespace

GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(IResourceManager& resourceManager, GUIRenderer& guiRenderer, const WindowSize& windowSize, GeneralTexture& texture)
    : GuiRendererElementBase(resourceManager, guiRenderer, GuiTextureElement::type, windowSize)
{
    texture_ = &texture;
}

void GuiTextureElement::Flip()
{
    scale_.y *= -1;
}
}  // namespace GameEngine
