#pragma once
#include "Types.h"

class CProjection
{
public:
    CProjection();
    CProjection(const wb::vec2i& window_size);
    CProjection(const wb::vec2i& window_size, float near, float far, float fov);
    const mat4& GetProjectionMatrix() const;
    mat4* GetProjectionMatrixPtr() { return &projectionMatrix; }
    const wb::vec2i& GetWindowSize();
    float GetViewDistance();
    void CreateProjectionMatrix();
    void OrthographicProjection();

private:	
    wb::vec2i windowSize;
    float nearPlane;
    float farPlane;
    float fov;
    mat4 projectionMatrix;
};
