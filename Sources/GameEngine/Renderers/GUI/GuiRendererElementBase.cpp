#include "GuiRendererElementBase.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(std::function<void(GuiElement&)> renderSubscribe,
                                               std::function<void(const GuiElement&)> unsubscribeElement,
                                               GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , texture_{nullptr}
    , color_(vec3(1.f))
    , unsubscribeElement_(unsubscribeElement)
{
    renderSubscribe(*this);
    SetOnchangeFunction([this]() { CalculateMatrix(); });
}
GuiRendererElementBase::~GuiRendererElementBase()
{
    unsubscribeElement_(*this);
}
void GuiRendererElementBase::SetColor(const vec3& color)
{
    color_ = color;
}
void GuiRendererElementBase::CalculateMatrix()
{
    transformMatrix_ = Utils::CreateTransformationMatrix(vec3(position_ + offset_, 0), scale_, 0.f);
}
}  // namespace GameEngine
