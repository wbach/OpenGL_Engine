#pragma once
#include "Plane.h"

namespace FrustrumPlane
{
enum Type
{
    FRUSTUM_NEAR = 0,
    FRUSTUM_FAR,
    FRUSTUM_LEFT,
    FRUSTUM_RIGHT,
    FRUSTUM_UP,
    FRUSTUM_DOWN
};
}
class CFrustrum
{
public:
    void CalculatePlanes(const mat4& projection_view_matrix);
    bool SphereIntersection(const vec3& center, const float& radius) const;

private:
    CPlane planes[6];
};
