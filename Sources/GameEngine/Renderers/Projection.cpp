#include "Projection.h"
#include "Utils.h"
#include "math.hpp"

CProjection::CProjection()
    : CProjection({640,480})
{}

CProjection::CProjection(const vec2i &window_size)
    : CProjection(window_size, 0.001f, 10000.f, 50.f)
{}

CProjection::CProjection(const vec2i &window_size, float near_, float far_, float fov_)
    : windowSize(window_size)
    , nearPlane(near_)
    , farPlane(far_)
    , fov(fov_)

{
    CreateProjectionMatrix();
}

CProjection::CProjection(const CProjection & p)
	: windowSize(p.windowSize)
	, nearPlane(p.nearPlane)
	, farPlane(p.farPlane)
	, fov(p.fov)
	, projectionMatrix(p.projectionMatrix)
	, mmutex()
{
}

CProjection& CProjection::operator=(const CProjection& p)
{
	windowSize	= p.windowSize;
	nearPlane	= p.nearPlane;
	farPlane	= p.farPlane;
	fov			= p.fov;
	projectionMatrix = p.projectionMatrix;
	return *this;
}

const mat4 &CProjection::GetProjectionMatrix()
{
	std::lock_guard<std::mutex> l(mmutex);
    return projectionMatrix;
}

const vec2i &CProjection::GetWindowSize()
{
	std::lock_guard<std::mutex> l(wmutex);
    return windowSize;
}


float CProjection::GetViewDistance() const
{
    return 0.8f * farPlane;
}

void CProjection::CreateProjectionMatrix()
{
    float aspect_ratio = (float)windowSize.x / (float)windowSize.y;
    float y_scale = (float)((1.0f / tan(Utils::ToRadians(fov / 2.0f))));
    float x_scale = y_scale / aspect_ratio;
    float frustum_length = farPlane - nearPlane;

    projectionMatrix[0][0] = x_scale;
    projectionMatrix[1][1] = y_scale;
    projectionMatrix[2][2] = -((farPlane + nearPlane) / frustum_length);
    projectionMatrix[2][3] = -1;
    projectionMatrix[3][2] = -((2 * nearPlane * farPlane) / frustum_length);
    projectionMatrix[3][3] = 0;
}

void CProjection::OrthographicProjection()
{
    float length = 100.f;
    projectionMatrix = mat4(1.f);
    projectionMatrix[0][0] = 2.f / windowSize.x;
    projectionMatrix[1][1] = 2.f / windowSize.y;
    projectionMatrix[2][2] = -2.f / length;
    projectionMatrix[3][3] = 1.f;
}
