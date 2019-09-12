#include "GuiRendererElementBase.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(std::function<void(GuiElement&)> renderSubscribe, std::function<void(const GuiElement&)> unsubscribeElement, GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , texture_{nullptr}
    , unsubscribeElement_(unsubscribeElement)
{
    renderSubscribe(*this);
}
GuiRendererElementBase::~GuiRendererElementBase()
{
    unsubscribeElement_(*this);
}
std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetId() : std::optional<uint32>();
}
}  // namespace GameEngine
