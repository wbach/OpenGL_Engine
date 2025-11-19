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
const float DEFAULT_FOV{60.f};
}  // namespace
Projection::Projection()
    : Projection({640, 480})
{
}
Projection::Projection(const vec2ui &renderingSize)
    : Projection(renderingSize, DEFAULT_NEAR_PLANE, EngineConf.renderer.viewDistance, DEFAULT_FOV)
{
}
Projection::Projection(const vec2ui &renderingSize, float near, float far, float fov)
    : renderingSize_(renderingSize.x, renderingSize.y)
    , aspectRatio_(CalculateAspectRatio())
    , nearPlane_(near)
    , farPlane_(far)
    , fov_(fov)
{
    Init();
}

Projection::Projection(const Projection &p)
    : renderingSize_(p.renderingSize_)
    , aspectRatio_(p.aspectRatio_)
    , nearPlane_(p.nearPlane_)
    , farPlane_(p.farPlane_)
    , fov_(p.fov_)
    , projectionMatrix_(p.projectionMatrix_)
{
    Init();
}
Projection::~Projection()
{
    UnsubscribeForEvents();
}
Projection &Projection::operator=(const Projection &p)
{
    UnsubscribeForEvents();

    aspectRatio_      = p.aspectRatio_;
    renderingSize_    = p.renderingSize_;
    nearPlane_        = p.nearPlane_;
    farPlane_         = p.farPlane_;
    fov_              = p.fov_;
    projectionMatrix_ = p.projectionMatrix_;

    Init();

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
    CreateProjectionMatrix();

    viewDistanceChangeSubscription_ = EngineConf.renderer.viewDistance.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "View distance change, recalculate projection matrix";
            farPlane_ = EngineConf.renderer.viewDistance;
            CreateProjectionMatrix();
        });

    resolutionChangeSubscription_ = EngineConf.renderer.resolution.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Rendering resolution change from: " << renderingSize_ << " to " << *EngineConf.renderer.resolution
                      << ", recalculate projection matrix";

            renderingSize_ = EngineConf.renderer.resolution;
            aspectRatio_   = CalculateAspectRatio();
            CreateProjectionMatrix();
        });
}

const mat4 &Projection::GetProjectionMatrix() const
{
    return projectionMatrix_;
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
void Projection::CreateProjectionMatrix()
{
    LOG_DEBUG << "Create projection matrix. FOV: " << fov_ << ", AR: " << aspectRatio_ << ", NEAR: " << nearPlane_
              << ", FAR: " << farPlane_;
    projectionMatrix_ = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
}
void Projection::OrthographiProjection()
{
    float length            = 100.f;
    projectionMatrix_       = mat4(1.f);
    projectionMatrix_[0][0] = 2.f / static_cast<float>(renderingSize_.x);
    projectionMatrix_[1][1] = 2.f / static_cast<float>(renderingSize_.y);
    projectionMatrix_[2][2] = -2.f / length;
    projectionMatrix_[3][3] = 1.f;
}
vec4 Projection::getBufferParams() const
{
    return vec4(nearPlane_, farPlane_, fov_, aspectRatio_);
}
}  // namespace GameEngine
