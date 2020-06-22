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
    //, shadowDistance_(projection.GetFar())
    , shadowDistance_(EngineConf.renderer.shadows.distance)
{
}

std::vector<vec4> ShadowBox::CalculateFrustumPoints()
{
    //float ar = projection_.GetAspectRatio();
    float ar = projection_.GetRenderingSize().y / projection_.GetRenderingSize().x;
    float tanHalfHFOV = tanf(glm::radians(projection_.GetFoV() / 2.0f));
    float tanHalfVFOV = tanf(glm::radians((projection_.GetFoV() * ar) / 2.0f));

    float xn = projection_.GetNear() * tanHalfHFOV;
    float xf = shadowDistance_ * tanHalfHFOV;

    float yn = projection_.GetNear() * tanHalfVFOV;
    float yf = shadowDistance_ * tanHalfVFOV;

    // clang-format off
    return
    {
        // near face
        vec4(xn, yn, projection_.GetNear(), 1.0),
        vec4(-xn, yn, projection_.GetNear(), 1.0),
        vec4(xn, -yn, projection_.GetNear(), 1.0),
        vec4(-xn, -yn,projection_.GetNear(), 1.0),

        // far face
        vec4(xf, yf, shadowDistance_, 1.0),
        vec4(-xf, yf, shadowDistance_, 1.0),
        vec4(xf, -yf, shadowDistance_, 1.0),
        vec4(-xf, -yf, shadowDistance_, 1.0)
    };
    // clang-format on

    // return CalculateFrustumVertices(cameraUpVector, cameraForwardVector, centerNear, centerFar);
}

mat4 ShadowBox::CreateLightViewMatrix(const Light& directionalLight)
{
    return glm::lookAt(vec3(0.f), directionalLight.GetDirection(), VECTOR_UP);
//    auto N = directionalLight.GetDirection();
//    auto U = glm::normalize(glm::cross(VECTOR_UP, N));
//    auto V = glm::cross(N, U);

//    //auto U2 = vec3(N.x, 0, N.z);
//    //U2.y = (-(N.x * N.x + N.z * N.z) / N.y);

//    //DEBUG_LOG("Dot : " + std::to_string(glm::dot(N, U)));
//    //DEBUG_LOG("Dot2 : " + std::to_string(glm::dot(N, U2)));

//    mat4 m(1.f);
//    // clang-format off
//    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
//    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
//    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
//    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
//    // clang-format on
//    return m;
}

mat4 ShadowBox::CreateOrthoProjTransform(const vec3& min, const vec3& max) const
{
    auto m1 = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);

    float l = min.x;
    float r = max.x;
    float b = min.y;
    float t = max.y;
    float n = min.z;
    float f = max.z;

    mat4 m(1.f);
//    // clang-format off
    m[0][0] = 2.0f / (r - l); m[0][1] = 0.0f;           m[0][2] = 0.0f;           m[0][3] = -(r + l) / (r - l);
    m[1][0] = 0.0f;           m[1][1] = 2.0f / (t - b); m[1][2] = 0.0f;           m[1][3] = -(t + b) / (t - b);
    m[2][0] = 0.0f;           m[2][1] = 0.0f;           m[2][2] = 2.0f / (f - n); m[2][3] = -(f + n) / (f - n);
    m[3][0] = 0.0f;           m[3][1] = 0.0f;           m[3][2] = 0.0f;           m[3][3] = 1.0;

   // mat4 m;
    // clang-format off
//    m[0][0] = 2.0f / (r - l); m[0][1] = 0.0f;           m[0][2] = 0.0f;           m[0][3] = 0.0f;
//    m[1][0] = 0.0f;           m[1][1] = 2.0f / (t - b); m[1][2] = 0.0f;           m[1][3] = 0.0f;
//    m[2][0] = 0.0f;           m[2][1] = 0.0f;           m[2][2] = 2.0f / (f - n); m[2][3] = 0.0f;
//    m[3][0] = -(r + l) / (r - l); m[3][1] = -(t + b) / (t - b);  m[3][2] = -(f + n) / (f - n);  m[3][3] = 1.0;
    // clang-format on

    DEBUG_LOG("glm: " + std::to_string(m1));
    DEBUG_LOG("m: " + std::to_string(glm::transpose(m)));
    return m1;
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
    //  auto lightViewMatrix = Utils::CreateLightViewMatrix(directionalLight.GetDirection(), vec3(0));

    vec3 min(std::numeric_limits<float>::max());
    vec3 max(-std::numeric_limits<float>::max());

    auto points = CalculateFrustumPoints();

    for (const vec4& point : points)
    {
        // Transform the frustum coordinate from view to world space
        auto pointInWorldSpace = invViewMatrix * point;

        // Transform the frustum coordinate from world to light space
        auto pointInLightSpace = lightViewMatrix * pointInWorldSpace;

        FindMinMax(pointInLightSpace, min, max);
    }

    lightProjectionViewMatrix_ = CreateOrthoProjTransform(min, max) * lightViewMatrix;
}
}  // namespace GameEngine
