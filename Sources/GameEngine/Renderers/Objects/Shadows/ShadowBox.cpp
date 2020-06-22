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
    , shadowDistance_(EngineConf.renderer.shadows.distance)
{
}

std::vector<vec4> ShadowBox::CalculateFrustumPoints()
{
    float ar = projection_.GetAspectRatio();
    float tanHalfHFOV = tanf(glm::radians(projection_.GetFoV() / 2.0f));
    float tanHalfVFOV = tanf(glm::radians((projection_.GetFoV() * ar) / 2.0f));

    float xn = projection_.GetNear() * tanHalfHFOV;
    float xf = shadowDistance_ * tanHalfHFOV;

    float yn = projection_.GetNear() * tanHalfVFOV;
    float yf = shadowDistance_ * tanHalfVFOV;

    // clang-format off
    return
    {
        // near face forward is z -1
        vec4(xn, yn, -projection_.GetNear(), 1.0),
        vec4(-xn, yn, -projection_.GetNear(), 1.0),
        vec4(xn, -yn, -projection_.GetNear(), 1.0),
        vec4(-xn, -yn, -projection_.GetNear(), 1.0),

        // far face
        vec4(xf, yf, -shadowDistance_, 1.0),
        vec4(-xf, yf, -shadowDistance_, 1.0),
        vec4(xf, -yf, -shadowDistance_, 1.0),
        vec4(-xf, -yf, -shadowDistance_, 1.0)
    };
    // clang-format on
}

mat4 ShadowBox::CreateLightViewMatrix(const Light& directionalLight)
{
    return glm::lookAt(vec3(0), directionalLight.GetPosition(), VECTOR_UP);
}

mat4 ShadowBox::CreateOrthoProjTransform(const vec3& min, const vec3& max) const
{
    //auto m1 = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
    //m1[2][2] *= -1.f; ???
    //return m1;

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

const mat4& ShadowBox::GetLightProjectionViewMatrix() const
{
    return lightProjectionViewMatrix_;
}

void ShadowBox::FindMinMax(const vec4& point, vec3& min, vec3& max)
{
    CheckMinMax(min.x, max.x, point.x);
    CheckMinMax(min.y, max.y, point.y);
    CheckMinMax(min.z, max.z, point.z);
}

void ShadowBox::CheckMinMax(float& min, float& max, float point)
{
    if (point > max)
    {
        max = point;
    }
    else if (point < min)
    {
        min = point;
    }
}

void ShadowBox::Update(const CameraWrapper& camera, const Light& directionalLight)
{
    auto invViewMatrix   = glm::inverse(camera.GetViewMatrix());
    auto lightViewMatrix = CreateLightViewMatrix(directionalLight);

    vec3 min(std::numeric_limits<float>::max());
    vec3 max(-std::numeric_limits<float>::max());

    auto points = CalculateFrustumPoints();

    for (const vec4& point : points)
    {
        auto pointInWorldSpace = invViewMatrix * point;
        auto pointInLightSpace = lightViewMatrix * pointInWorldSpace;

        FindMinMax(pointInLightSpace, min, max);
    }
    lightProjectionViewMatrix_ = CreateOrthoProjTransform(min, max) * lightViewMatrix;
}
}  // namespace GameEngine
