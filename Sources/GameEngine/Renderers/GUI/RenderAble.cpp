#include "RenderAble.h"

#include <Logger/Log.h>
#include <Utils.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "GameEngine/Renderers/GUI/Renderer.h"

namespace GameEngine
{
namespace GUI
{
RenderAble::RenderAble(IResourceManager& resourceManager, Renderer& renderer)
    : resourceManager_(resourceManager)
    , renderer_(renderer)
    , texture_{nullptr}
    , color_(vec4(1.f))
    , transformMatrix_(mat4(1.f))
{
    updateTransformMatrix();
    renderer_.Subscribe(*this);
}

RenderAble::~RenderAble()
{
    renderer_.UnSubscribe(id);
    deleteTexture();
}

void RenderAble::setColor(const Color& color)
{
    color_ = color;
}
const mat4& RenderAble::getTransformMatrix() const
{
    return transformMatrix_;
}
std::optional<uint32> RenderAble::getTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}
const Color& RenderAble::getColor() const
{
    return color_;
}
const GeneralTexture* RenderAble::getTexture() const
{
    return texture_;
}
void RenderAble::updateTransformMatrix()
{
    // convert from range 0.f - 1.f to -1.f - 1.f
    // api rendering quad -1 - 1f  (*2f not needed)
    transformMatrix_ = Utils::CreateTransformationMatrix(getScreenPosition() * 2.f - 1.f, getScreenScale() , DegreesFloat(0.f));
}
void RenderAble::updateTexture()
{
}
void RenderAble::deleteTexture()
{
    if (texture_)
    {
        resourceManager_.GetTextureLoader().DeleteTexture(*texture_);
        texture_ = nullptr;
    }
}
void RenderAble::onRender()
{
    releaseTimerStart_.reset();

    updateTexture();
    updateTransformMatrix();
}
void RenderAble::onSkipRender()
{
    if (texture_ and inactivityReleaseTime > 0)
    {
        if (not releaseTimerStart_)
        {
            releaseTimerStart_ = std::chrono::high_resolution_clock::now();
            return;
        }

        if (releaseTimerStart_)
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - *releaseTimerStart_).count() > inactivityReleaseTime)
            {
                LOG_DEBUG << "Gui texture [GPU id: " << texture_->GetGpuObjectId() << "] inactivity timeout. Release texture";
                deleteTexture();
                releaseTimerStart_.reset();
            }
        }
    }
}
void RenderAble::inactivityRelease(int v)
{
    inactivityReleaseTime = v;
}
int RenderAble::inactivityRelease() const
{
    return inactivityReleaseTime;
}
}  // namespace GUI
}  // namespace GameEngine
