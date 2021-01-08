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
{
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

void GuiRendererElementBase::SetLocalScale(const vec2& scale)
{
    GuiElement::SetLocalScale(scale);
}

void GuiRendererElementBase::SetLocalPostion(const vec2& position)
{
    GuiElement::SetLocalPostion(position);
}

void GuiRendererElementBase::SetScreenScale(const vec2& scale)
{
    GuiElement::SetScreenScale(scale);
}

void GuiRendererElementBase::SetScreenPostion(const vec2& position)
{
    GuiElement::SetScreenPostion(position);
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
void GuiRendererElementBase::setParent(GuiElement* parent)
{
    GuiElement::setParent(parent);
}

mat4 GuiRendererElementBase::GetTransformMatrix() const
{
    // convert from range 0.f - 1.f to -1.f - 1.f
    // api rendering quad -1 - 1f  (*2f not needed)
    return Utils::CreateTransformationMatrix(GetScreenPosition() * 2.f - 1.f, GetScreenScale(),
                                             DegreesFloat(0.f));
}
}  // namespace GameEngine
