#include "ShadowBox.h"

#include <GLM/GLMUtils.h>
#include <Glm.h>
#include <Logger/Log.h>
#include <Utils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/BoundingBox.h"

namespace GameEngine
{
ShadowBox::ShadowBox(const Projection& projection)
    : projection_(projection)
    , shadowDistance_(EngineConf.renderer.shadows.distance.get() /
                      static_cast<float>(EngineConf.renderer.shadows.cascadesSize.get()))
{
    calculateTangentHalfFov();
    caclulateCascadeDistances();
}

std::vector<vec4> ShadowBox::calculateFrustumPoints(float near, float far)
{
    float xn = near * tanHalfFov_.x;
    float xf = far * tanHalfFov_.x;

    float yn = near * tanHalfFov_.y;
    float yf = far * tanHalfFov_.y;

    near *= VECTOR_FORWARD.z;
    far *= VECTOR_FORWARD.z;

    // clang-format off
    return
    {
        // near face forward is z -1
        vec4(xn, yn, near, 1.0),
        vec4(-xn, yn, near, 1.0),
        vec4(xn, -yn, near, 1.0),
        vec4(-xn, -yn, near, 1.0),

        // far face
        vec4(xf, yf, far, 1.0),
        vec4(-xf, yf, far, 1.0),
        vec4(xf, -yf, far, 1.0),
        vec4(-xf, -yf, far, 1.0)
    };
    // clang-format on
}

mat4 ShadowBox::createLightViewMatrix(const Light& directionalLight)
{
    return glm::lookAt(vec3(0), directionalLight.GetPosition(), VECTOR_UP);
}

mat4 ShadowBox::createOrthoProjTransform(const vec3& min, const vec3& max) const
{
    // auto m1 = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
    // m1[2][2] *= -1.f; ???
    // return m1;

    float l = min.x;
    float r = max.x;
    float b = min.y;
    float t = max.y;
    float n = min.z;
    float f = max.z;

    mat4 m(1.f);

    // clang-format off
    m[0][0] = 2.0f / (r - l); m[0][1] = 0.0f;           m[0][2] = 0.0f;           m[0][3] = 0.0f;
    m[1][0] = 0.0f;           m[1][1] = 2.0f / (t - b); m[1][2] = 0.0f;           m[1][3] = 0.0f;
    m[2][0] = 0.0f;           m[2][1] = 0.0f;           m[2][2] = 2.0f / (f - n); m[2][3] = 0.0f;
    m[3][0] = -(r + l) / (r - l); m[3][1] = -(t + b) / (t - b);  m[3][2] = -(f + n) / (f - n);  m[3][3] = 1.0;
    // clang-format on
    return m;
}

const mat4* ShadowBox::getLightProjectionViewMatrices() const
{
    return lightProjectionViewMatrices_;
}

const float* ShadowBox::getLightCascadeDistances() const
{
    return cascadeDistances_;
}

void expDistances(float* cascadeDistances)
{
    float a = pow(EngineConf.renderer.shadows.distance.get() , 1.f / (EngineConf.renderer.shadows.cascadesSize.get() - 1));

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = pow(a, i);
        DEBUG_LOG("Cascade : " + std::to_string(cascadeDistances[i]));
    }
}

void quadraticDistances(float* cascadeDistances)
{
    float s0 = *EngineConf.renderer.shadows.firstCascadeDistance;
    float a  = (*EngineConf.renderer.shadows.distance - s0) / pow(*EngineConf.renderer.shadows.cascadesSize - 1, 2);

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = a * pow(i, 2) + s0;
        DEBUG_LOG("Cascade : " + std::to_string(cascadeDistances[i]));
    }
}

void linearDistances(float* cascadeDistances)
{
    float s0 = *EngineConf.renderer.shadows.firstCascadeDistance;
    float a  = (*EngineConf.renderer.shadows.distance - s0) / (*EngineConf.renderer.shadows.cascadesSize - 1);

    for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
    {
        cascadeDistances[i] = a * i + s0;
        DEBUG_LOG("Cascade : " + std::to_string(cascadeDistances[i]));
    }
}

void ShadowBox::caclulateCascadeDistances()
{
    if (EngineConf.renderer.shadows.cascadesSize.get() == 1)
    {
        for (uint32 i = 0; i < Params::MAX_SHADOW_MAP_CASADES; ++i)
        {
            cascadeDistances_[i] = *EngineConf.renderer.shadows.distance;
            DEBUG_LOG("Cascade : " + std::to_string(cascadeDistances_[i]));
        }
        return;
    }

    switch (EngineConf.renderer.shadows.cascadeDistanceFunc)
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

void ShadowBox::calculateTangentHalfFov()
{
    auto halfFov  = projection_.GetFoV() / 2.0f;
    tanHalfFov_.x = tanf(glm::radians(halfFov));
    tanHalfFov_.y = tanf(glm::radians(halfFov * projection_.GetAspectRatio()));
}

void ShadowBox::checkMinMax(const vec4& point, vec3& min, vec3& max)
{
    checkMinMax(min.x, max.x, point.x);
    checkMinMax(min.y, max.y, point.y);
    checkMinMax(min.z, max.z, point.z);
}

void ShadowBox::checkMinMax(float& min, float& max, float point)
{
    if (point > max)
    {
        max = point;
    }

    if (point < min)
    {
        min = point;
    }
}

void ShadowBox::update(const CameraWrapper& camera, const Light& directionalLight)
{
    auto invViewMatrix   = glm::inverse(camera.GetViewMatrix());
    auto lightViewMatrix = createLightViewMatrix(directionalLight);

    for (uint32 cascadeIndex = 0; cascadeIndex < *EngineConf.renderer.shadows.cascadesSize; ++cascadeIndex)
    {
        vec3 min(std::numeric_limits<float>::max());
        vec3 max(-std::numeric_limits<float>::max());

        float near  = cascadeIndex == 0 ? projection_.GetNear() : cascadeDistances_[cascadeIndex - 1];
        float far   = cascadeDistances_[cascadeIndex];
        auto points = calculateFrustumPoints(near, far);

        for (const vec4& point : points)
        {
            auto pointInWorldSpace = invViewMatrix * point;
            auto pointInLightSpace = lightViewMatrix * pointInWorldSpace;

            checkMinMax(pointInLightSpace, min, max);
        }
        auto bias                                  = .1f * (max - min);
        lightProjectionViewMatrices_[cascadeIndex] = createOrthoProjTransform(min - bias, max + bias) * lightViewMatrix;
    }
}
}  // namespace GameEngine
