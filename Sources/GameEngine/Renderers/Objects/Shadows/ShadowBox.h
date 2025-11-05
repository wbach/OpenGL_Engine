#pragma once
#include <Glm.h>

#include <array>
#include <vector>

#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
class Light;
class Projection;

class ShadowBox
{
public:
    ShadowBox(const Projection&);
    void update(const CameraWrapper&, const Light&);
    const mat4* getLightProjectionViewMatrices() const;
    const float* getLightCascadeDistances() const;

private:
    void caclulateCascadeDistances();
    void calculateTangentHalfFov();
    void checkMinMax(const vec4& point, vec3& min, vec3& max);
    void checkMinMax(float& min, float& max, float point);
    std::vector<vec4> calculateFrustumPoints(float near, float far);
    mat4 createLightViewMatrix(const Light&, const CameraWrapper&);
    mat4 createOrthoProjTransform(const vec3& min, const vec3& max) const;

private:
    const Projection& projection_;
    mat4 lightProjectionViewMatrices_[Params::MAX_SHADOW_MAP_CASADES];
    float cascadeDistances_[Params::MAX_SHADOW_MAP_CASADES];
    float shadowDistance_;
    vec2 tanHalfFov_;
};
}  // namespace GameEngine
