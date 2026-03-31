#include "GuiTextureElement.h"

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/Texture.h"
namespace GameEngine
{
GuiElementTypes GuiTextureElement::type = GuiElementTypes::Texture;

GuiTextureElement::GuiTextureElement(IResourceManager& resourceManager, GUIRenderer& guiRenderer, GeneralTexture& texture)
    : GuiRendererElementBase(resourceManager, guiRenderer, GuiTextureElement::type)
{
    texture_ = &texture;
}

GuiTextureElement::GuiTextureElement(IResourceManager& resourceManager, GUIRenderer& guiRenderer)
    : GuiRendererElementBase(resourceManager, guiRenderer, GuiTextureElement::type)
{
}

void GuiTextureElement::Flip()
{
    transform_.scale.y *= -1;
}
void GuiTextureElement::SetTexture(const File& file)
{
    if (texture_)
    {
        resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
    }

    TextureParameters params;
    params.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;
    texture_               = resourceManager_.GetTextureLoader().LoadTexture(file, params);
}
}  // namespace GameEngine
