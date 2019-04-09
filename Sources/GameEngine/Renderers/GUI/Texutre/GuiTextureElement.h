#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class GuiTextureElement : public GuiElement
{
public:
    GuiTextureElement(const vec2ui& windowSize, const Texture& texture);
    const Texture& GetTexture() const;
    void Flip();

private:
    const Texture& texture_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
