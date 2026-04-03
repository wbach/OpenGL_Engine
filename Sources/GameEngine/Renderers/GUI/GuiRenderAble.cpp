#include "GuiRenderAble.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Utils.h"

namespace GameEngine
{
GuiRenderAble::GuiRenderAble(IResourceManager& resourceManager, GUIRenderer& guiRenderer, GuiElementTypes type)
    : GuiElement(type)
    , resourceManager_(resourceManager)
    , guiRenderer_(guiRenderer)
    , texture_{nullptr}
    , color_(vec4(1.f))
    , transformMatrix_(mat4(1.f))
{
    guiRenderer_.Subscribe(*this);
    UpdateTransformMatrix();
}

GuiRenderAble::~GuiRenderAble()
{
    guiRenderer_.UnSubscribe(*this);
    DeleteTexture();
}

void GuiRenderAble::SetColor(const Color& color)
{
    color_ = color;
}
const mat4& GuiRenderAble::GetTransformMatrix() const
{
    return transformMatrix_;
}
std::optional<uint32> GuiRenderAble::GetTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}
const Color& GuiRenderAble::GetColor() const
{
    return color_;
}
const GeneralTexture* GuiRenderAble::GetTexture() const
{
    return texture_;
}
void GuiRenderAble::UpdateTransformMatrix()
{
    // convert from range 0.f - 1.f to -1.f - 1.f
    // api rendering quad -1 - 1f  (*2f not needed)
    transformMatrix_ = Utils::CreateTransformationMatrix(GetScreenPosition() * 2.f - 1.f, GetScreenScale(), DegreesFloat(0.f));
}
void GuiRenderAble::UpdateTexture()
{
}
void GuiRenderAble::DeleteTexture()
{
    if (texture_)
    {
        resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
        texture_ = nullptr;
    }
}
void GuiRenderAble::OnRender()
{
    releaseTimerStart_.reset();

    UpdateTexture();
    UpdateTransformMatrix();
}
void GuiRenderAble::OnSkipRender()
{
    if (texture_ and inactivityRelease > 0)
    {
        if (not releaseTimerStart_)
        {
            releaseTimerStart_ = std::chrono::high_resolution_clock::now();
            return;
        }

        if (releaseTimerStart_)
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - *releaseTimerStart_).count() > inactivityRelease)
            {
                LOG_DEBUG << "Gui texture [GPU id: " << texture_->GetGpuObjectId() << "] inactivity timeout. Release texture";
                DeleteTexture();
                releaseTimerStart_.reset();
            }
        }
    }
}
void GuiRenderAble::InactivityRelease(int v)
{
    inactivityRelease = v;
}
int GuiRenderAble::InactivityRelease() const
{
    return inactivityRelease;
}
}  // namespace GameEngine
