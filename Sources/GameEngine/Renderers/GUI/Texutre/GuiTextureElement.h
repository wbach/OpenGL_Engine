#pragma once
#include "GameEngine/Renderers/GUI/GuiRenderAble.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class ENGINE_API GuiTextureElement : public GuiRenderAble
{
public:
    GuiTextureElement(IResourceManager&, GUIRenderer&, GeneralTexture&);
    GuiTextureElement(IResourceManager&, GUIRenderer&);
    void Flip();
    void SetTexture(const File&);

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
