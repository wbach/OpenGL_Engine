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
{
    calculateTangentHalfFov();
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

const mat4& ShadowBox::getLightProjectionViewMatrix() const
{
    return lightProjectionViewMatrix_;
}

void ShadowBox::calculateTangentHalfFov()
{
    auto halfFov = projection_.GetFoV() / 2.0f;
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

    vec3 min(std::numeric_limits<float>::max());
    vec3 max(-std::numeric_limits<float>::max());

    auto points = calculateFrustumPoints(projection_.GetNear(), EngineConf.renderer.shadows.distance);

    for (const vec4& point : points)
    {
        auto pointInWorldSpace = invViewMatrix * point;
        auto pointInLightSpace = lightViewMatrix * pointInWorldSpace;

        checkMinMax(pointInLightSpace, min, max);
    }
    lightProjectionViewMatrix_ = createOrthoProjTransform(min, max) * lightViewMatrix;
}
}  // namespace GameEngine
