#include "GuiRendererElementBase.h"
#include <Logger/Log.h>
#include "GameEngine/Renderers/GUI/GuiRenderer.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(GUIRenderer& guiRenderer,
                                               GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , guiRenderer_(guiRenderer)
    , texture_{nullptr}
    , color_(vec4(1.f))
    , offset_(0)
    , transformMatrix_(1.f)
{
    CalculateMatrix();
    guiRenderer_.Subscribe(*this);
}

GuiRendererElementBase::~GuiRendererElementBase()
{
    guiRenderer_.UnSubscribe(*this);
}

void GuiRendererElementBase::SetScale(const vec2 &scale)
{
    GuiElement::SetScale(scale);
    CalculateMatrix();
}

void GuiRendererElementBase::SetPostion(const vec2 &position)
{
    GuiElement::SetPostion(position);
    CalculateMatrix();
}
void GuiRendererElementBase::SetColor(const vec3& color)
{
    color_.x = color.x;
    color_.y = color.y;
    color_.z = color.z;
    color_.w = 1.f;
}
void GuiRendererElementBase::SetColor(const vec4& color)
{
    color_ = color;
}
void GuiRendererElementBase::CalculateMatrix()
{
    transformMatrix_ = Utils::CreateTransformationMatrix(position_ + offset_, scale_, DegreesFloat(0.f));
}
}  // namespace GameEngine
