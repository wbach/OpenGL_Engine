#include "GuiRendererElementBase.h"
#include <Logger/Log.h>
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(IResourceManager& resourceManager, GUIRenderer& guiRenderer,
                                               GuiElementTypes type)
    : GuiElement(type)
    , resourceManager_(resourceManager)
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

    if (texture_)
    {
        resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
    }
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
