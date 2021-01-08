#include "GuiTextureElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace
{
const std::string emptyString = "";
} // namespace

GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(IResourceManager& resourceManager, GUIRenderer& guiRenderer, GeneralTexture& texture)
    : GuiRendererElementBase(resourceManager, guiRenderer, GuiTextureElement::type)
{
    texture_ = &texture;
}

void GuiTextureElement::Flip()
{
    transform_.scale.y *= -1;
}
}  // namespace GameEngine
