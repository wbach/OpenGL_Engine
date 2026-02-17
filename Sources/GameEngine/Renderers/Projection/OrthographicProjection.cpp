#include "OrthographicProjection.h"

#include <GLM/GLMUtils.h>

namespace GameEngine
{
void OrthographicProjection::UpdateMatrix()
{
    float halfHeight = zoom * REFERENCE_ORTHO_DISTANCE;
    float halfWidth  = halfHeight * GetAspectRatio();

    matrix_ = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane_, farPlane_);
}
void OrthographicProjection::SetZoom(float z)
{
    zoom = std::max(0.01f, z);
}
float OrthographicProjection::IncreaseZoom(float dz)
{
    zoom += dz;
    zoom = std::max(0.01f, zoom);
    return zoom;
}
float OrthographicProjection::DecreaseZoom(float dz)
{
    zoom -= dz;
    zoom = std::max(0.01f, zoom);
    return zoom;
}
ProjectionType OrthographicProjection::GetType() const
{
    return ProjectionType::Orthographic;
}
std::unique_ptr<IProjection> OrthographicProjection::Clone() const
{
    return std::make_unique<OrthographicProjection>(renderingSize_, nearPlane_, farPlane_);
}
}  // namespace GameEngine
