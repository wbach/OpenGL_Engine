#pragma once
#include "Types.h"
#include <mutex>

class CProjection
{
public:
    CProjection();
    CProjection(const vec2i& window_size);
    CProjection(const vec2i& window_size, float near, float far, float fov);
	CProjection(const CProjection& p);
	CProjection& operator=(const CProjection& p);

    void CreateProjectionMatrix();
    void OrthographicProjection();

    float GetViewDistance() const;
    float GetFoV() const {return fov;}
    float GetFar() const {return farPlane;}
    float GetNear() const {return nearPlane;}
    const vec2i& GetWindowSize();
    const mat4& GetProjectionMatrix();

private:
    vec2i windowSize;
    float nearPlane;
    float farPlane;
    float fov;
    mat4 projectionMatrix;
	std::mutex mmutex;
	std::mutex wmutex;
};
