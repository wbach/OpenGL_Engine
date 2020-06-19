#pragma once
#include <memory>
#include <vector>

#include "GameEngine/Camera/CameraWrapper.h"
#include "glm/glm.hpp"

namespace GameEngine
{
class Projection;

class ShadowBox
{
public:
    ShadowBox(const Projection&);
    void Update(const CameraWrapper&);
    vec3 GetCenter() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetLength() const;
    void CalculateMatrixes(const vec3& lightDirection);
    const mat4& GetProjectionViewMatrix() const;
    const mat4& GetViewMatrix() const;
    const mat4& GetProjectionMatrix() const;

private:
    void FindMinMax(const vec4& point);
    void CheckMinMax(float& min, float& max, float point);
    void CalculateWidthsAndHeights();
    std::vector<vec4> CalculateFrustumPoints(const CameraWrapper&);
    std::vector<vec4> CalculateFrustumVertices(const vec3&, const vec3&, const vec3&, const vec3&) const;
    vec4 CalculateLightSpaceFrustumCorner(const vec3& startPoint, const vec3& direction, float width) const;

private:
    const Projection& projection_;
    mat4 projectionMatrix_ = mat4(1.f);
    mat4 viewMatrix_       = mat4(1.f);
    vec3 m_Min;
    vec3 m_Max;

    mat4 projectionViewMatrix;

    float m_FarHeight, m_FarWidth, m_NearHeight, m_NearWidth;

    float m_Offset;
    float m_ShadowDistance;
};
}  // namespace GameEngine
