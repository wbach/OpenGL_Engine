#include "GuiRendererElementBase.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"

namespace GameEngine
{
GuiRendererElementBase::GuiRendererElementBase(IResourceManager& resourceManager, GUIRenderer& guiRenderer, GuiElementTypes type)
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
        texture_ = nullptr;
    }
}

void GuiRendererElementBase::SetLocalScale(const vec2& scale)
{
    GuiElement::SetLocalScale(scale);
}

void GuiRendererElementBase::SetLocalPosition(const vec2& position)
{
    GuiElement::SetLocalPosition(position);
}

void GuiRendererElementBase::SetScreenScale(const vec2& scale)
{
    GuiElement::SetScreenScale(scale);
}

void GuiRendererElementBase::SetScreenPostion(const vec2& position)
{
    GuiElement::SetScreenPostion(position);
}

void GuiRendererElementBase::SetColor(const Color& color)
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
    return Utils::CreateTransformationMatrix(GetScreenPosition() * 2.f - 1.f, GetScreenScale(), DegreesFloat(0.f));
}
std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}
const Color& GuiRendererElementBase::GetColor() const
{
    return color_;
}
const GeneralTexture* GuiRendererElementBase::GetTexture() const
{
    return texture_;
}
}  // namespace GameEngine
