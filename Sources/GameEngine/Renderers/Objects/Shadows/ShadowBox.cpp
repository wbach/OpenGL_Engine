#include "ShadowBox.h"

#include <GLM/GLMUtils.h>
#include <Glm.h>
#include <Logger/Log.h>
#include <Utils.h>

#include <magic_enum/magic_enum.hpp>

#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/Projection/PerspectiveProjection.h"

namespace GameEngine
{
ShadowBox::ShadowBox()
    : shadowDistance_(EngineConf.renderer.shadows.distance.get() /
                      static_cast<float>(EngineConf.renderer.shadows.cascadesSize.get()))
{
    lightProjectionViewMatrices_.resize(Params::MAX_SHADOW_MAP_CASADES);
    cascadeDistances_.resize(Params::MAX_SHADOW_MAP_CASADES);

    caclulateCascadeDistances();
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    return frustumCorners;
}

const std::vector<mat4>& ShadowBox::getLightProjectionViewMatrices() const
{
    return lightProjectionViewMatrices_;
}

const std::vector<float>& ShadowBox::getLightCascadeDistances() const
{
    return cascadeDistances_;
}

void expDistances(std::vector<float>& cascadeDistances)
{
    float a = pow(EngineConf.renderer.shadows.distance.get(),
                  1.f / (static_cast<float>(EngineConf.renderer.shadows.cascadesSize.get()) - 1.f));

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = powf(a, static_cast<float>(i));
        LOG_DEBUG << "Cascade : " << cascadeDistances[i];
    }
}

void quadraticDistances(std::vector<float>& cascadeDistances)
{
    float s0 = *EngineConf.renderer.shadows.firstCascadeDistance;
    float a  = (*EngineConf.renderer.shadows.distance - s0) /
              powf(static_cast<float>(*EngineConf.renderer.shadows.cascadesSize) - 1.f, 2.f);

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = a * powf(static_cast<float>(i), 2.f) + s0;
        LOG_DEBUG << "Cascade : " << cascadeDistances[i];
    }
}

void linearDistances(std::vector<float>& cascadeDistances)
{
    float s0 = *EngineConf.renderer.shadows.firstCascadeDistance;
    float a =
        (*EngineConf.renderer.shadows.distance - s0) / (static_cast<float>(*EngineConf.renderer.shadows.cascadesSize) - 1.f);

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = a * i + s0;
        LOG_DEBUG << "Cascade : " << cascadeDistances[i];
    }
}

void ShadowBox::caclulateCascadeDistances()
{
    auto cascaedsSize = EngineConf.renderer.shadows.cascadesSize.get();
    LOG_DEBUG << "CaclulateCascadeDistances : " << cascaedsSize;

    if (cascaedsSize == 1)
    {
        for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
        {
            cascadeDistances_[i] = *EngineConf.renderer.shadows.distance;
            LOG_DEBUG << "Cascade : " << cascadeDistances_[i];
        }
        return;
    }

    LOG_DEBUG << "Calculate shadow distance by function: "
              << magic_enum::enum_name(EngineConf.renderer.shadows.cascadeDistanceFunc.get());

    switch (EngineConf.renderer.shadows.cascadeDistanceFunc.get())
    {
        case Params::Shadows::CascadeDistanceFunc::linear:
            linearDistances(cascadeDistances_);
            break;
        case Params::Shadows::CascadeDistanceFunc::quadratic:
            quadraticDistances(cascadeDistances_);
            break;
        case Params::Shadows::CascadeDistanceFunc::exp:
            expDistances(cascadeDistances_);
            break;
    }
}

mat4 calculateLightSpaceMatrix(const std::vector<vec4>& corners, const vec3& lightDir)
{
    vec3 center = vec3(0.0f);
    for (const auto& v : corners)
    {
        center += vec3(v);
    }
    center /= static_cast<float>(corners.size());

    float radius = 0.0f;
    for (const auto& v : corners)
    {
        float distance = glm::length(vec3(v) - center);
        radius         = std::max(radius, distance);
    }

    // Zaokrąglamy promień do góry, aby uniknąć mikro-skoków przy zmianie precyzji
    radius = std::ceil(radius * 16.0f) / 16.0f;

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = -radius;
    float maxX = radius;
    float minY = -radius;
    float maxY = radius;

    // Stabilizacja (Texel Snapping)
    float worldUnitsPerTexel = (maxX - minX) / EngineConf.renderer.shadows.mapSize;

    minX = std::floor(minX / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxX = std::floor(maxX / worldUnitsPerTexel) * worldUnitsPerTexel;
    minY = std::floor(minY / worldUnitsPerTexel) * worldUnitsPerTexel;
    maxY = std::floor(maxY / worldUnitsPerTexel) * worldUnitsPerTexel;

    // Near plane to light source
    float nearMargin = 150.0f;
    float minZ       = -radius - nearMargin;
    float maxZ       = radius;

    const glm::mat4 lightOrtho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightOrtho * lightView;
}

void ShadowBox::update(const ICamera& camera, const vec3& lightDir)
{
    const auto perspective = dynamic_cast<const PerspectiveProjection*>(&camera.GetProjection());
    if (not perspective)
    {
        return;
    }

    for (uint32 cascadeIndex = 0; cascadeIndex < *EngineConf.renderer.shadows.cascadesSize; ++cascadeIndex)
    {
        float near = cascadeIndex == 0 ? camera.GetProjection().GetNear() : cascadeDistances_[cascadeIndex - 1];
        float far  = cascadeDistances_[cascadeIndex];

        glm::mat4 cascadeProj = glm::perspective(glm::radians(perspective->GetFoV()), perspective->GetAspectRatio(), near, far);

        auto corners                               = getFrustumCornersWorldSpace(cascadeProj, camera.GetViewMatrix());
        lightProjectionViewMatrices_[cascadeIndex] = calculateLightSpaceMatrix(corners, lightDir);
    }
}
}  // namespace GameEngine
