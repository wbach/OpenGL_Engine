#pragma once
#include <Glm.h>

#include <vector>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.h"

namespace GameEngine
{
class Light;

class ShadowBox
{
public:
    ShadowBox();
    void update(const ICamera&, const Light&);
    const mat4* getLightProjectionViewMatrices() const;
    const float* getLightCascadeDistances() const;

private:
    void caclulateCascadeDistances();
    vec2 calculateTangentHalfFov(const ICamera&);
    void checkMinMax(const vec4& point, vec3& min, vec3& max);
    void checkMinMax(float& min, float& max, float point);
    std::vector<vec4> calculateFrustumPoints(const ICamera&, float near, float far);
    mat4 createLightViewMatrix(const Light&, const ICamera&);
    mat4 createOrthoProjTransform(const vec3& min, const vec3& max) const;

private:
    mat4 lightProjectionViewMatrices_[Params::MAX_SHADOW_MAP_CASADES];
    float cascadeDistances_[Params::MAX_SHADOW_MAP_CASADES];
    float shadowDistance_;
};
}  // namespace GameEngine
