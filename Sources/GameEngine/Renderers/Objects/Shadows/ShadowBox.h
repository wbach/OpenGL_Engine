#pragma once
#include <Glm.h>

#include <memory>
#include <vector>

#include "GameEngine/Camera/CameraWrapper.h"

namespace GameEngine
{
class Light;
class Projection;

class ShadowBox
{
public:
    ShadowBox(const Projection&);
    void Update(const CameraWrapper&, const Light&);
    const mat4& GetLightProjectionViewMatrix() const;

private:
    void FindMinMax(const vec4& point, vec3& min, vec3& max);
    void CheckMinMax(float& min, float& max, float point);
    std::vector<vec4> CalculateFrustumPoints();
    mat4 CreateLightViewMatrix(const Light&);
    mat4 CreateOrthoProjTransform(const vec3& min, const vec3& max) const;

private:
    const Projection& projection_;
    mat4 lightProjectionViewMatrix_;
    float shadowDistance_;
};
}  // namespace GameEngine
