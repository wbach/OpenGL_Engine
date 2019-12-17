#include "Projection.h"
#include "Mutex.hpp"
#include "Utils.h"
#include "math.hpp"

namespace GameEngine
{
namespace
{
std::mutex mmutex;
std::mutex wmutex;
}  // namespace
Projection::Projection()
    : Projection({640, 480})
{
}
Projection::Projection(const vec2ui &renderingSize)
    : Projection(renderingSize, 0.1f, 20000.f, 50.f)
{
}
Projection::Projection(const vec2ui &renderingSize, float near, float far, float fov)
    : renderingSize_(renderingSize.x / 2, renderingSize.y / 2)
    , nearPlane_(near)
    , farPlane_(far)
    , fov_(fov)
{
    CreateProjectionMatrix();
}
Projection::Projection(const Projection &p)
    : renderingSize_(p.renderingSize_)
    , nearPlane_(p.nearPlane_)
    , farPlane_(p.farPlane_)
    , fov_(p.fov_)
    , projectionMatrix_(p.projectionMatrix_)
{
}
Projection &Projection::operator=(const Projection &p)
{
    renderingSize_    = p.renderingSize_;
    nearPlane_        = p.nearPlane_;
    farPlane_         = p.farPlane_;
    fov_              = p.fov_;
    projectionMatrix_ = p.projectionMatrix_;
    return *this;
}
const mat4 &Projection::GetProjectionMatrix() const
{
    std::lock_guard<std::mutex> l(mmutex);
    return projectionMatrix_;
}
const vec2ui &Projection::GetRenderingSize() const
{
    std::lock_guard<std::mutex> l(wmutex);
    return renderingSize_;
}
void Projection::CreateProjectionMatrix()
{
    float aspect_ratio   = static_cast<float>(renderingSize_.x) / static_cast<float>(renderingSize_.y);
    float y_scale        = (1.0f / tanf(Utils::ToRadians(fov_ / 2.0f)));
    float x_scale        = y_scale / aspect_ratio;
    float frustum_length = farPlane_ - nearPlane_;

    projectionMatrix_[0][0] = x_scale;
    projectionMatrix_[1][1] = y_scale;
    projectionMatrix_[2][2] = -((farPlane_ + nearPlane_) / frustum_length);
    projectionMatrix_[2][3] = -1;
    projectionMatrix_[3][2] = -((2 * nearPlane_ * farPlane_) / frustum_length);
    projectionMatrix_[3][3] = 0;
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
