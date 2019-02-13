#include "ShadowBox.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"
#include "Utils.h"

namespace GameEngine
{
ShadowBox::ShadowBox(Projection& projection)
    : m_WindowSize(projection.GetWindowSize())
    , m_Min()
    , m_Max()
    , viewMatrix_(1.f)
    , m_Fov(projection.GetFoV())
    , m_NearPlane(projection.GetNear())
    , m_Offset(1.f)
    , m_Up(0, 1, 0, 0)
    , m_Forward(0, 0, -1, 0)
    , m_ShadowDistance(EngineConf.renderer.shadows.distance)
{
    CalculateWidthsAndHeights();
}

void ShadowBox::CalculateWidthsAndHeights()
{
    m_FarWidth   = (float)(m_ShadowDistance * tan(Utils::ToRadians(m_Fov)));
    m_NearWidth  = (float)(m_NearPlane * tan(Utils::ToRadians(m_Fov)));
    m_FarHeight  = m_FarWidth / GetAspectRatio();
    m_NearHeight = m_NearWidth / GetAspectRatio();
}

std::vector<vec4> ShadowBox::CalculateFrustumPoints(ICamera* camera)
{
    mat4 rotation = CalculateCameraRotationMatrix(camera);
    vec3 forward_vector(rotation * m_Forward);
    vec3 to_far(forward_vector);
    to_far *= m_ShadowDistance;

    vec3 to_near(forward_vector);
    to_near *= m_NearPlane;

    vec3 center_near = to_near + camera->GetPosition();
    vec3 center_far  = to_far + camera->GetPosition();

    return CalculateFrustumVertices(rotation, forward_vector, center_near, center_far);
}

const float ShadowBox::GetAspectRatio() const
{
    return static_cast<float>(m_WindowSize.x) / static_cast<float>(m_WindowSize.y);
}

void ShadowBox::CalculateMatrixes(const vec3& lightDirection)
{
    viewMatrix_          = Utils::CreateLightViewMatrix(lightDirection, GetCenter());
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
    CheckMinMax(m_Max.x, m_Min.x, point.x);
    CheckMinMax(m_Max.y, m_Min.y, point.y);
    CheckMinMax(m_Max.z, m_Min.z, point.z);
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

mat4 ShadowBox::CalculateCameraRotationMatrix(ICamera* camera) const
{
    mat4 rotation(1.f);
    rotation *= glm::rotate((float)(-camera->GetYaw()), vec3(0.f, 1.f, 0.f));
    rotation *= glm::rotate((float)(-camera->GetPitch()), vec3(1.f, 0.f, 0.f));
    return rotation;
}

void ShadowBox::Update(ICamera* camera)
{
    if (camera == nullptr)
    {
        return;
    }

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
    m_Max.z += m_Offset;
}

vec3 ShadowBox::GetCenter() const
{
    float x = (m_Min.x + m_Max.x) / 2.f;
    float y = (m_Min.y + m_Max.y) / 2.f;
    float z = (m_Min.z + m_Max.z) / 2.f;
    vec4 cen(x, y, z, 1);
    mat4 inverted_light = glm::inverse(viewMatrix_);
    return vec3(inverted_light * cen);
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

std::vector<vec4> ShadowBox::CalculateFrustumVertices(mat4 rotation, vec3 forward_vector, vec3 center_near,
                                                      vec3 center_far) const
{
    vec3 up_vector(rotation * m_Up);
    vec3 right_vector = glm::cross(forward_vector, up_vector);
    vec3 down_vector(-up_vector.x, -up_vector.y, -up_vector.z);
    vec3 left_vector(-right_vector.x, -right_vector.y, -right_vector.z);
    vec3 far_top     = center_far + up_vector * m_FarHeight;
    vec3 far_bottom  = center_far + down_vector * m_FarHeight;
    vec3 near_top    = center_near + up_vector * m_NearHeight;
    vec3 near_bottom = center_near + down_vector * m_NearHeight;

    std::vector<vec4> points = {CalculateLightSpaceFrustumCorner(far_top, right_vector, m_FarWidth),
                                CalculateLightSpaceFrustumCorner(far_top, left_vector, m_FarWidth),
                                CalculateLightSpaceFrustumCorner(far_bottom, right_vector, m_FarWidth),
                                CalculateLightSpaceFrustumCorner(far_bottom, left_vector, m_FarWidth),
                                CalculateLightSpaceFrustumCorner(near_top, right_vector, m_NearWidth),
                                CalculateLightSpaceFrustumCorner(near_top, left_vector, m_NearWidth),
                                CalculateLightSpaceFrustumCorner(near_bottom, right_vector, m_NearWidth),
                                CalculateLightSpaceFrustumCorner(near_bottom, left_vector, m_NearWidth)};
    return points;
}

vec4 ShadowBox::CalculateLightSpaceFrustumCorner(const vec3& startPoint, const vec3& direction,
                                                 const float& width) const
{
    vec3 point = startPoint + vec3(direction.x * width, direction.y * width, direction.z * width);
    vec4 point4f(point.x, point.y, point.z, 1.0f);
    point4f = viewMatrix_ * point4f;
    return point4f;
}
}  // namespace GameEngine
