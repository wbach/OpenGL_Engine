#include "Projection.h"

#include <GLM/GLMUtils.h>
#include <Logger/Log.h>
#include <Utils.h>

#include <Mutex.hpp>
#include <math.hpp>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
namespace
{
std::mutex mmutex;
std::mutex wmutex;

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
    CreateProjectionMatrix();

    viewDistanceChangeSubscription_ = EngineConf.renderer.viewDistance.subscribeForChange(
        [this]()
        {
            farPlane_ = EngineConf.renderer.viewDistance;
            CreateProjectionMatrix();
        });
}
Projection::Projection(const Projection &p)
    : renderingSize_(p.renderingSize_)
    , aspectRatio_(p.aspectRatio_)
    , nearPlane_(p.nearPlane_)
    , farPlane_(p.farPlane_)
    , fov_(p.fov_)
    , projectionMatrix_(p.projectionMatrix_)
{
}
Projection::~Projection()
{
    EngineConf.renderer.viewDistance.unsubscribe(viewDistanceChangeSubscription_);
}
Projection &Projection::operator=(const Projection &p)
{
    aspectRatio_      = p.aspectRatio_;
    renderingSize_    = p.renderingSize_;
    nearPlane_        = p.nearPlane_;
    farPlane_         = p.farPlane_;
    fov_              = p.fov_;
    projectionMatrix_ = p.projectionMatrix_;
    return *this;
}

void Projection::SetRenderingSize(const vec2ui& newSize)
{
    renderingSize_ = newSize;
    CalculateAspectRatio();
    CreateProjectionMatrix();
}

void Projection::Init()
{
    renderingSize_ = EngineConf.renderer.resolution.get();
    aspectRatio_   = CalculateAspectRatio();
    CreateProjectionMatrix();
}
const mat4 &Projection::GetProjectionMatrix() const
{
    std::lock_guard<std::mutex> l(mmutex);
    return projectionMatrix_;
}

float Projection::CalculateAspectRatio() const
{
    return static_cast<float>(renderingSize_.x) / static_cast<float>(renderingSize_.y);
}

const vec2ui &Projection::GetRenderingSize() const
{
    std::lock_guard<std::mutex> l(wmutex);
    return renderingSize_;
}
void Projection::CreateProjectionMatrix()
{
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
}  // namespace GameEngine
