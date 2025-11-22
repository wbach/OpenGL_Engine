#include "Projection.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>
#include <Utils.h>

#include <math.hpp>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
namespace
{
const float DEFAULT_NEAR_PLANE{.3f};

}  // namespace
Projection::Projection()
    : Projection({640, 480})
{
}
Projection::Projection(const vec2ui &renderingSize)
    : Projection(renderingSize, DEFAULT_NEAR_PLANE, EngineConf.renderer.viewDistance)
{
}
Projection::Projection(const vec2ui &renderingSize, float near, float far)
    : renderingSize_(renderingSize.x, renderingSize.y)
    , aspectRatio_(CalculateAspectRatio())
    , nearPlane_(near)
    , farPlane_(far)
{
}

Projection::Projection(const Projection &p)
    : renderingSize_(p.renderingSize_)
    , aspectRatio_(p.aspectRatio_)
    , nearPlane_(p.nearPlane_)
    , farPlane_(p.farPlane_)
    , matrix_(p.matrix_)
{
}
Projection::~Projection()
{
    UnsubscribeForEvents();
}
Projection &Projection::operator=(const Projection &p)
{
    UnsubscribeForEvents();

    aspectRatio_   = p.aspectRatio_;
    renderingSize_ = p.renderingSize_;
    nearPlane_     = p.nearPlane_;
    farPlane_      = p.farPlane_;
    matrix_        = p.matrix_;

    return *this;
}

void Projection::UnsubscribeForEvents()
{
    if (viewDistanceChangeSubscription_)
        EngineConf.renderer.viewDistance.unsubscribe(*viewDistanceChangeSubscription_);

    if (resolutionChangeSubscription_)
        EngineConf.renderer.resolution.unsubscribe(*resolutionChangeSubscription_);

    viewDistanceChangeSubscription_.reset();
    resolutionChangeSubscription_.reset();
}

void Projection::Init()
{
    renderingSize_ = EngineConf.renderer.resolution.get();
    aspectRatio_   = CalculateAspectRatio();
    UpdateMatrix();

    viewDistanceChangeSubscription_ = EngineConf.renderer.viewDistance.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "View distance change, recalculate projection matrix";
            farPlane_ = EngineConf.renderer.viewDistance;
            UpdateMatrix();
        });

    resolutionChangeSubscription_ = EngineConf.renderer.resolution.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Rendering resolution change from: " << renderingSize_ << " to " << *EngineConf.renderer.resolution
                      << ", recalculate projection matrix";

            renderingSize_ = EngineConf.renderer.resolution;
            aspectRatio_   = CalculateAspectRatio();
            UpdateMatrix();
        });
}

const mat4 &Projection::GetMatrix() const
{
    return matrix_;
}

float Projection::CalculateAspectRatio() const
{
    auto result = static_cast<float>(renderingSize_.x) / static_cast<float>(renderingSize_.y);
    LOG_DEBUG << "New aspect ratio: " << result;
    return result;
}

const vec2ui &Projection::GetRenderingSize() const
{
    return renderingSize_;
}

vec4 Projection::GetBufferParams() const
{
    return vec4(nearPlane_, farPlane_, renderingSize_.x, renderingSize_.y);
}

float Projection::GetViewDistance() const
{
    return 0.95f * farPlane_;
}

float Projection::GetFar() const
{
    return farPlane_;
}

float Projection::GetNear() const
{
    return nearPlane_;
}

float Projection::GetAspectRatio() const
{
    return aspectRatio_;
}
}  // namespace GameEngine
