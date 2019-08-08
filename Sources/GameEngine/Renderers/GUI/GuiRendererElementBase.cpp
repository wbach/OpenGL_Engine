#include "GuiRendererElementBase.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , texture_{nullptr}
{
}
std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetId() : std::optional<uint32>();
}
}  // namespace GameEngine
