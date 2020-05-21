#pragma once
#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class GuiTextureElement : public GuiRendererElementBase
{
public:
    GuiTextureElement(IResourceManager&, GUIRenderer&, const vec2ui&, Texture&);
    void Flip();

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
