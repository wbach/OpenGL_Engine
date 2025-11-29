#include <GLM/GLMUtils.h>

#include "OrthographicProjection.h"

namespace GameEngine
{
void OrthographicProjection::UpdateMatrix()
{
    float halfHeight = zoom * 10.f; // np. 10 jednostek świata
    float halfWidth  = halfHeight * GetAspectRatio();

    matrix_ = glm::ortho(-halfWidth, halfWidth,
                         -halfHeight, halfHeight,
                         nearPlane_, farPlane_);
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
}  // namespace GameEngine
