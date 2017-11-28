#pragma once
#include "Types.h"

class CProjection
{
public:
    CProjection();
    CProjection(const vec2i& window_size);
    CProjection(const vec2i& window_size, float near, float far, float fov);

    void CreateProjectionMatrix();
    void OrthographicProjection();

    float GetViewDistance() const;
    float GetFoV() const {return fov;}
    float GetFar() const {return farPlane;}
    float GetNear() const {return nearPlane;}
    const vec2i& GetWindowSize() const;
    const mat4& GetProjectionMatrix() const;
    mat4* GetProjectionMatrixPtr() { return &projectionMatrix; }

private:
    vec2i windowSize;
    float nearPlane;
    float farPlane;
    float fov;
    mat4 projectionMatrix;
};
