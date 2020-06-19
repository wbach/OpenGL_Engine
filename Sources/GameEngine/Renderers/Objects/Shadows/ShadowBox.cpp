#include "ShadowBox.h"

#include <GLM/GLMUtils.h>
#include <Glm.h>
#include <Logger/Log.h>
#include <Utils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
ShadowBox::ShadowBox(const Projection& projection)
    : projection_(projection)
    , viewMatrix_(1.f)
    , m_Min()
    , m_Max()
    , m_Offset(1.f)
    , m_ShadowDistance(EngineConf.renderer.shadows.distance)
{
    CalculateWidthsAndHeights();
}

void ShadowBox::CalculateWidthsAndHeights()
{
    m_FarWidth =
        m_ShadowDistance * 2.f * tanf(ToRadians(projection_.GetFoV() / 2.f));  // // 2 * shdowDistance * tanf(fov /2) ?
    m_NearWidth =
        projection_.GetNear() * 2.f * tanf(ToRadians(projection_.GetFoV() / 2.f));  // 2 * near * tanf(fov /2) ?
    m_FarHeight  = m_FarWidth / projection_.GetAspectRatio();
    m_NearHeight = m_NearWidth / projection_.GetAspectRatio();
}

std::vector<vec4> ShadowBox::CalculateFrustumPoints(const CameraWrapper& camera)
{
    //{
    const auto& viewMatrix         = camera.GetViewMatrix();
    const auto cameraForwardVector = glm::normalize(vec3(viewMatrix[2]));
    const auto cameraUpVector      = glm::normalize(vec3(viewMatrix[1]));
    //    DEBUG_LOG("cameraForwardVector " + std::to_string(cameraForwardVector));
    //    DEBUG_LOG("cameraUpVector " + std::to_string(cameraUpVector));
    ////}
    //{
    //    auto cameraRotation      = camera.GetRotation().value_;
    //    vec3 cameraForwardVector = glm::normalize(cameraRotation * VECTOR_FORWARD);
    //    vec3 cameraUpVector      = glm::normalize(cameraRotation * VECTOR_UP);
    //    DEBUG_LOG("cameraForwardVector " + std::to_string(cameraForwardVector));
    //    DEBUG_LOG("cameraUpVector " + std::to_string(cameraUpVector));
    //}
    vec3 toFarVector = cameraForwardVector * m_ShadowDistance;
    vec3 toNear      = cameraForwardVector * projection_.GetNear();

    vec3 centerNear = camera.GetPosition() + toNear;
    vec3 centerFar  = camera.GetPosition() + toFarVector;

    return CalculateFrustumVertices(cameraUpVector, cameraForwardVector, centerNear, centerFar);
}

void ShadowBox::CalculateMatrixes(const vec3& lightDirection)
{
    viewMatrix_ =
        Utils::CreateLightViewMatrix(lightDirection, GetCenter());
    projectionMatrix_    = Utils::CreateOrthoProjectionMatrix(GetWidth(), GetHeight(), GetLength());
    projectionViewMatrix = projectionMatrix_ * viewMatrix_;
}

const mat4& ShadowBox::GetProjectionViewMatrix() const
{
    return projectionViewMatrix;
}

const mat4& ShadowBox::GetViewMatrix() const
{
    return viewMatrix_;
}

const mat4& ShadowBox::GetProjectionMatrix() const
{
    return projectionMatrix_;
}

void ShadowBox::FindMinMax(const vec4& point)
{
    CheckMinMax(m_Min.x, m_Max.x, point.x);
    CheckMinMax(m_Min.y, m_Max.y, point.y);
    CheckMinMax(m_Min.z, m_Max.z, point.z);
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

void ShadowBox::Update(const CameraWrapper& camera)
{
    bool first = true;
    for (const vec4& point : CalculateFrustumPoints(camera))
    {
        if (first)
        {
            m_Min.x = point.x;
            m_Max.x = point.x;
            m_Min.y = point.y;
            m_Max.y = point.y;
            m_Min.z = point.z;
            m_Max.z = point.z;
            first   = false;
            continue;
        }
        FindMinMax(point);
    }
    // m_Max.z += m_Offset;
}

vec3 ShadowBox::GetCenter() const
{
    vec4 cen((m_Min + m_Max) / 2.f, 1.f);
    return cen;
}

float ShadowBox::GetWidth() const
{
    return m_Max.x - m_Min.x;
}

float ShadowBox::GetHeight() const
{
    return m_Max.y - m_Min.y;
}

float ShadowBox::GetLength() const
{
    return m_Max.z - m_Min.z;
}

std::vector<vec4> ShadowBox::CalculateFrustumVertices(const vec3& cameraUpVector, const vec3& cameraForwardVector,
                                                      const vec3& centerNear, const vec3& centerFar) const
{
    vec3 cameraRightVector = glm::cross(cameraForwardVector, cameraUpVector);
    vec3 cameraDownVector  = -1.f * cameraUpVector;
    vec3 cameraLeftVector  = -1.f * cameraRightVector;

    auto halfFarHeight  = m_FarHeight * .5f;
    auto halfNearHeight = m_NearHeight * .5f;
    vec3 far_top        = centerFar + (cameraUpVector * halfFarHeight);
    vec3 far_bottom     = centerFar + (cameraDownVector * halfFarHeight);
    vec3 near_top       = centerNear + (cameraUpVector * halfNearHeight);
    vec3 near_bottom    = centerNear + (cameraDownVector * halfNearHeight);

    auto halfFarWidth  = m_FarWidth * .5f;
    auto halfNearWidth = m_NearWidth * .5f;

    return {CalculateLightSpaceFrustumCorner(far_top, cameraRightVector, halfFarWidth),
            CalculateLightSpaceFrustumCorner(far_top, cameraLeftVector, halfFarWidth),
            CalculateLightSpaceFrustumCorner(far_bottom, cameraRightVector, halfFarWidth),
            CalculateLightSpaceFrustumCorner(far_bottom, cameraLeftVector, halfFarWidth),
            CalculateLightSpaceFrustumCorner(near_top, cameraRightVector, halfNearWidth),
            CalculateLightSpaceFrustumCorner(near_top, cameraLeftVector, halfNearWidth),
            CalculateLightSpaceFrustumCorner(near_bottom, cameraRightVector, halfNearWidth),
            CalculateLightSpaceFrustumCorner(near_bottom, cameraLeftVector, halfNearWidth)};
}

vec4 ShadowBox::CalculateLightSpaceFrustumCorner(const vec3& startPoint, const vec3& direction, float width) const
{
    return vec4(startPoint + (direction * width), 1.f);
}
}  // namespace GameEngine
