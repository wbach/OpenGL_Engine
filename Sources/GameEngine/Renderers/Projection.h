#pragma once
#include "Types.h"

namespace GameEngine
{
class Projection
{
public:
    Projection();
    Projection(const vec2ui& window_size);
    Projection(const vec2ui& window_size, float near, float far, float fov);
    Projection(const Projection& p);
    Projection& operator=(const Projection& p);

    void CreateProjectionMatrix();
    void OrthographiProjection();

    inline float GetViewDistance() const;
    inline float GetFoV() const;
    inline float GetFar() const;
    inline float GetNear() const;
    const vec2ui& GetWindowSize() const;
    const mat4& GetProjectionMatrix() const;

private:
    vec2ui windowSize;
    float nearPlane;
    float farPlane;
    float fov;
    mat4 projectionMatrix;
};

float Projection::GetViewDistance() const
{
    return 0.8f * farPlane;
}
float Projection::GetFoV() const
{
    return fov;
}

float Projection::GetFar() const
{
    return farPlane;
}

float Projection::GetNear() const
{
    return nearPlane;
}
}  // namespace GameEngine
