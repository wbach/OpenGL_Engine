#pragma once
#include <Glm.h>

#include <vector>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/ConfigurationParams/RendererParams/ShadowsParams/ShadowsParams.h"

namespace GameEngine
{
namespace Components
{
class DirectionalLightComponent;
}

class ShadowBox
{
public:
    ShadowBox();
    void update(const ICamera&, const vec3&);
    const std::vector<mat4>& getLightProjectionViewMatrices() const;
    const std::vector<float>& getLightCascadeDistances() const;

private:
    void caclulateCascadeDistances();

private:
    std::vector<mat4> lightProjectionViewMatrices_;
    std::vector<float> cascadeDistances_;
    float shadowDistance_;
};
}  // namespace GameEngine
