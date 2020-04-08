#pragma once
#include "GameEngine/Renderers/GUI/GuiRendererElementBase.h"
#include "Types.h"

namespace GameEngine
{
class Texture;
class GuiTextureElement : public GuiRendererElementBase
{
public:
    GuiTextureElement(GUIRenderer&, const vec2ui& windowSize, Texture& texture);
    void Flip();
    const std::string& GetFilename() const;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
