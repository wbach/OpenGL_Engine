#pragma once
#include "glm/glm.hpp"
#include "../Utils/Utils.h"

class SProjection
{
public:
	SProjection()
		: SProjection({640,480}, .1f, 1000.f, 50.f)
	{}
	SProjection(const wb::vec2i& window_size)
		: SProjection(window_size, .1f, 1000.f, 50.f)
	{}
	SProjection(const wb::vec2i& window_size, float near, float far, float fov)
		: m_WindowSize(window_size)
		, m_NearPlane(near)
		, m_FarPlane(far)
		, m_Fov(fov)
	{
		CreateProjectionMatrix();
	}
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	glm::mat4* GetProjectionMatrixPtr() { return &m_ProjectionMatrix; }
	const wb::vec2i& GetWindowSize() {return m_WindowSize;}
	float GetViewDistance() { return 0.8f * m_FarPlane; }
	void CreateProjectionMatrix()
	{
		float aspect_ratio = (float)m_WindowSize.x / (float)m_WindowSize.y;
		float y_scale = (float)((1.0f / tan(Utils::ToRadians(m_Fov / 2.0f))));
		float x_scale = y_scale / aspect_ratio;
		float frustum_length = m_FarPlane - m_NearPlane;

		m_ProjectionMatrix[0][0] = x_scale;
		m_ProjectionMatrix[1][1] = y_scale;
		m_ProjectionMatrix[2][2] = -((m_FarPlane + m_NearPlane) / frustum_length);
		m_ProjectionMatrix[2][3] = -1;
		m_ProjectionMatrix[3][2] = -((2 * m_NearPlane * m_FarPlane) / frustum_length);
		m_ProjectionMatrix[3][3] = 0;
	}
	void OrthographicProjection()
	{
		float length = 100.f;
		m_ProjectionMatrix = glm::mat4(1.f);
		m_ProjectionMatrix[0][0] = 2.f / m_WindowSize.x;
		m_ProjectionMatrix[1][1] = 2.f / m_WindowSize.y;
		m_ProjectionMatrix[2][2] = -2.f / length;
		m_ProjectionMatrix[3][3] = 1.f;
	}
private:	
	float m_NearPlane;
	float m_FarPlane;
	float m_Fov;
	wb::vec2i m_WindowSize;	
	glm::mat4 m_ProjectionMatrix;
};