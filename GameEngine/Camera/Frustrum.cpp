#include "Frustrum.h"
#include "../Utils/Utils.h"
void CFrustrum::CalculatePlanes(const glm::mat4& projection_view_matrix)
{
	m_Planes[FrustrumPlane::FRUSTUM_RIGHT].m_Normal.x		 = projection_view_matrix[0][3] - projection_view_matrix[0][0];
	m_Planes[FrustrumPlane::FRUSTUM_RIGHT].m_Normal.y		 = projection_view_matrix[1][3] - projection_view_matrix[1][0];
	m_Planes[FrustrumPlane::FRUSTUM_RIGHT].m_Normal.z		 = projection_view_matrix[2][3] - projection_view_matrix[2][0];
	m_Planes[FrustrumPlane::FRUSTUM_RIGHT].m_DistanceToOrgin = projection_view_matrix[3][3] - projection_view_matrix[3][0];

	m_Planes[FrustrumPlane::FRUSTUM_LEFT].m_Normal.x		= projection_view_matrix[0][3] + projection_view_matrix[0][0];
	m_Planes[FrustrumPlane::FRUSTUM_LEFT].m_Normal.y		= projection_view_matrix[1][3] + projection_view_matrix[1][0];
	m_Planes[FrustrumPlane::FRUSTUM_LEFT].m_Normal.z		= projection_view_matrix[2][3] + projection_view_matrix[2][0];
	m_Planes[FrustrumPlane::FRUSTUM_LEFT].m_DistanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][0];

	m_Planes[FrustrumPlane::FRUSTUM_DOWN].m_Normal.x		= projection_view_matrix[0][3] + projection_view_matrix[0][1];
	m_Planes[FrustrumPlane::FRUSTUM_DOWN].m_Normal.y		= projection_view_matrix[1][3] + projection_view_matrix[1][1];
	m_Planes[FrustrumPlane::FRUSTUM_DOWN].m_Normal.z		= projection_view_matrix[2][3] + projection_view_matrix[2][1];
	m_Planes[FrustrumPlane::FRUSTUM_DOWN].m_DistanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][1];

	m_Planes[FrustrumPlane::FRUSTUM_UP].m_Normal.x		  = projection_view_matrix[0][3] - projection_view_matrix[0][1];
	m_Planes[FrustrumPlane::FRUSTUM_UP].m_Normal.y		  = projection_view_matrix[1][3] - projection_view_matrix[1][1];
	m_Planes[FrustrumPlane::FRUSTUM_UP].m_Normal.z		  = projection_view_matrix[2][3] - projection_view_matrix[2][1];
	m_Planes[FrustrumPlane::FRUSTUM_UP].m_DistanceToOrgin = projection_view_matrix[3][3] - projection_view_matrix[3][1];

	m_Planes[FrustrumPlane::FRUSTUM_FAR].m_Normal.x		   = projection_view_matrix[0][3] - projection_view_matrix[0][2];
	m_Planes[FrustrumPlane::FRUSTUM_FAR].m_Normal.y		   = projection_view_matrix[1][3] - projection_view_matrix[1][2];
	m_Planes[FrustrumPlane::FRUSTUM_FAR].m_Normal.z		   = projection_view_matrix[2][3] - projection_view_matrix[2][2];
	m_Planes[FrustrumPlane::FRUSTUM_FAR].m_DistanceToOrgin = projection_view_matrix[3][3] - projection_view_matrix[3][2];

	m_Planes[FrustrumPlane::FRUSTUM_NEAR].m_Normal.x		= projection_view_matrix[0][3] + projection_view_matrix[0][2];
	m_Planes[FrustrumPlane::FRUSTUM_NEAR].m_Normal.y		= projection_view_matrix[1][3] + projection_view_matrix[1][2];
	m_Planes[FrustrumPlane::FRUSTUM_NEAR].m_Normal.z		= projection_view_matrix[2][3] + projection_view_matrix[2][2];
	m_Planes[FrustrumPlane::FRUSTUM_NEAR].m_DistanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][2];

	// Normalize all m_Planeslane normals
	for (int i = 0; i < 6; i++)
		m_Planes[i].Normalize();
}

bool CFrustrum::SphereIntersection(const glm::vec3& center, const float& radius)
{	
	for (int i = 0; i < 6; i++)
	{
		// Plane-sphere intersection test. If p*n + d + r < 0 then we're outside the plane.
		if (glm::dot(center, m_Planes[i].m_Normal) + m_Planes[i].m_DistanceToOrgin + radius <= 0)
			return false;
	}
	return true;
}
