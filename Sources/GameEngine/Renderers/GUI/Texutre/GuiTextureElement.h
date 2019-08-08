#pragma once
#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class GuiTextureElement : public GuiRendererElementBase
{
public:
    GuiTextureElement(const vec2ui& windowSize, Texture& texture);
    void Flip();

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
