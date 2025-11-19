#pragma once
#include <optional>
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
    ~Projection();

    Projection& operator=(const Projection& p);

    void CreateProjectionMatrix();
    void OrthographiProjection();

    inline float GetViewDistance() const;
    inline float GetFoV() const;
    inline float GetFar() const;
    inline float GetNear() const;
    inline float GetAspectRatio() const;
    const vec2ui& GetRenderingSize() const;
    const mat4& GetProjectionMatrix() const;
    vec4 getBufferParams() const;

private:
    float CalculateAspectRatio() const;
    void Init();
    void UnsubscribeForEvents();

private:
    vec2ui renderingSize_;
    float aspectRatio_;
    float nearPlane_;
    float farPlane_;
    float fov_;
    mat4 projectionMatrix_;
    std::optional<IdType> viewDistanceChangeSubscription_;
    std::optional<IdType> resolutionChangeSubscription_;
};

float Projection::GetViewDistance() const
{
    return 0.95f * farPlane_;
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

float Projection::GetAspectRatio() const
{
    return aspectRatio_;
}
}  // namespace GameEngine
