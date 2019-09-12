#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
class Texture;

class GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, GuiElementTypes type, const vec2ui& windowSize);
    ~GuiRendererElementBase() override;
    std::optional<uint32> GetTextureId() const;

protected:
    Texture* texture_;
    std::function<void(const GuiElement&)> unsubscribeElement_;
};
}  // namespace GameEngine
