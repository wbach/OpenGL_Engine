#pragma once
#include "Types.h"

namespace GameEngine
{
class Projection
{
public:
    Projection();
    Projection(const vec2ui& renderingSize);
    Projection(const vec2ui& renderingSize, float near, float far, float fov);
    Projection(const Projection& p);
    Projection& operator=(const Projection& p);

    void CreateProjectionMatrix();
    void OrthographiProjection();

    inline float GetViewDistance() const;
    inline float GetFoV() const;
    inline float GetFar() const;
    inline float GetNear() const;
    const vec2ui& GetRenderingSize() const;
    const mat4& GetProjectionMatrix() const;

private:
    vec2ui renderingSize_;
    float nearPlane_;
    float farPlane_;
    float fov_;
    mat4 projectionMatrix_;
};

float Projection::GetViewDistance() const
{
    return 0.4f * farPlane_;
}
float Projection::GetFoV() const
{
    return fov_;
}

float Projection::GetFar() const
{
    return farPlane_;
}

float Projection::GetNear() const
{
    return nearPlane_;
}
}  // namespace GameEngine
