#include "Sprite.h"

#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
namespace GUI
{
Sprite::Sprite(IResourceManager& resourceManager, Renderer& renderer, GeneralTexture& texture)
    : RenderAble(resourceManager, renderer)
{
    texture_ = &texture;
    inactivityRelease(0);
}

Sprite::Sprite(IResourceManager& resourceManager, Renderer& renderer)
    : RenderAble(resourceManager, renderer)
{
    inactivityRelease(0);
}

Sprite::Sprite(const Sprite& other)
    : RenderAble(other)
{
    if (auto texture = other.getTexture())
    {
        if (auto file = texture->GetFile())
        {
            texture_ = resourceManager_.GetTextureLoader().LoadTexture(*file, texture->getTextureParameters());
        }
    }
    inactivityRelease(0);
}

void Sprite::Flip()
{
    transform.scale.y *= -1;
}
void Sprite::SetTexture(const FileHandle& file)
{
    if (texture_)
    {
        resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
        texture_ = nullptr;
    }

    if (file.empty())
        return;

    TextureParameters params;
    params.sizeLimit = std::nullopt;;
    params.flipMode        = TextureFlip::VERTICAL;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;

    texture_ = resourceManager_.GetTextureLoader().LoadTexture(file, params);
}
void Sprite::accept(IElementVisitor& visitor) const
{
    visitor.visit(*this);
}
std::unique_ptr<Element> Sprite::clone() const
{
    return std::make_unique<Sprite>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
