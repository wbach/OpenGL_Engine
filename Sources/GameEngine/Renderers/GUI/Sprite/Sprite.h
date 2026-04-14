#pragma once
#include "GameEngine/Renderers/GUI/RenderAble.h"
#include "Types.h"

namespace GameEngine
{
class Texture;

namespace GUI
{
class ENGINE_API Sprite : public RenderAble
{
public:
    Sprite(IResourceManager&, Renderer&, GeneralTexture&);
    Sprite(IResourceManager&, Renderer&);

    void Flip();
    void SetTexture(const FileHandle&);

private:
    void accept(IElementVisitor&) override;
};
}  // namespace GUI
}  // namespace GameEngine
