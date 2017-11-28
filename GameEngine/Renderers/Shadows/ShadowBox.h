#pragma once
#include <memory>
#include <vector>
#include "Camera/Camera.h"
#include "glm/glm.hpp"

class CProjection;

class CShadowBox
{
public:
    CShadowBox(CProjection*);
    void Update(CCamera*);
    vec3 GetCenter() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetLength() const;
    const float GetAspectRatio() const;
    void SetLightViewMatrix(const mat4& matrix);

private:
    void FindMinMax(const vec4& point);
    void CheckMinMax(float& min, float& max, float point);
    void CalculateWidthsAndHeights();
    std::vector<vec4> CalculateFrustumPoints(CCamera*);
    mat4 CalculateCameraRotationMatrix(CCamera* camera) const;
    std::vector<vec4> CalculateFrustumVertices(mat4 rotation, vec3 forward_vector, vec3 center_near, vec3 center_far) const;
    vec4 CalculateLightSpaceFrustumCorner(const vec3& startPoint, const vec3& direction, const float& width) const;

private:
    vec2i m_WindowSize;
    vec3 m_Min;
    vec3 m_Max;

    mat4 m_LightViewMatrix;

    float m_FarHeight, m_FarWidth, m_NearHeight, m_NearWidth;
    float m_Fov, m_NearPlane;

    float m_Offset;
    vec4 m_Up;
    vec4 m_Forward;
    float m_ShadowDistance;
};
