#include <GLM/GLMUtils.h>

#include "OrthographicProjection.h"

namespace GameEngine
{
void OrthographicProjection::UpdateMatrix()
{
    float w = static_cast<float>(renderingSize_.x);
    float h = static_cast<float>(renderingSize_.y);

    float left   = -w * 0.5f;
    float right  = w * 0.5f;
    float bottom = -h * 0.5f;
    float top    = h * 0.5f;

    matrix_ = glm::ortho(left, right, bottom, top, nearPlane_, farPlane_);
}
}  // namespace GameEngine
