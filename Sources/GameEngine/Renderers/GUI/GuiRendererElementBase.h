#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
class Texture;

class GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(GuiElementTypes type, const vec2ui& windowSize);
    std::optional<uint32> GetTextureId() const;

protected:
    Texture* texture_;
};
}  // namespace GameEngine
