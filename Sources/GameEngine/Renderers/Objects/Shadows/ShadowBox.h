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
    void update(const CameraWrapper&, const Light&);
    const mat4& getLightProjectionViewMatrix() const;

private:
    void calculateTangentHalfFov();
    void checkMinMax(const vec4& point, vec3& min, vec3& max);
    void checkMinMax(float& min, float& max, float point);
    std::vector<vec4> calculateFrustumPoints(float near, float far);
    mat4 createLightViewMatrix(const Light&);
    mat4 createOrthoProjTransform(const vec3& min, const vec3& max) const;

private:
    const Projection& projection_;
    mat4 lightProjectionViewMatrix_;
    vec2 tanHalfFov_;
};
}  // namespace GameEngine
