#include "Frustrum.h"
#include "Utils.h"

void CFrustrum::CalculatePlanes(const glm::mat4& projection_view_matrix)
{
    planes[FrustrumPlane::FRUSTUM_RIGHT].normal.x		   = projection_view_matrix[0][3] - projection_view_matrix[0][0];
    planes[FrustrumPlane::FRUSTUM_RIGHT].normal.y		   = projection_view_matrix[1][3] - projection_view_matrix[1][0];
    planes[FrustrumPlane::FRUSTUM_RIGHT].normal.z		   = projection_view_matrix[2][3] - projection_view_matrix[2][0];
    planes[FrustrumPlane::FRUSTUM_RIGHT].distanceToOrgin = projection_view_matrix[3][3] - projection_view_matrix[3][0];

    planes[FrustrumPlane::FRUSTUM_LEFT].normal.x		  = projection_view_matrix[0][3] + projection_view_matrix[0][0];
    planes[FrustrumPlane::FRUSTUM_LEFT].normal.y		  = projection_view_matrix[1][3] + projection_view_matrix[1][0];
    planes[FrustrumPlane::FRUSTUM_LEFT].normal.z		  = projection_view_matrix[2][3] + projection_view_matrix[2][0];
    planes[FrustrumPlane::FRUSTUM_LEFT].distanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][0];

    planes[FrustrumPlane::FRUSTUM_DOWN].normal.x		  = projection_view_matrix[0][3] + projection_view_matrix[0][1];
    planes[FrustrumPlane::FRUSTUM_DOWN].normal.y		  = projection_view_matrix[1][3] + projection_view_matrix[1][1];
    planes[FrustrumPlane::FRUSTUM_DOWN].normal.z		  = projection_view_matrix[2][3] + projection_view_matrix[2][1];
    planes[FrustrumPlane::FRUSTUM_DOWN].distanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][1];

    planes[FrustrumPlane::FRUSTUM_UP].normal.x		  = projection_view_matrix[0][3] - projection_view_matrix[0][1];
    planes[FrustrumPlane::FRUSTUM_UP].normal.y		  = projection_view_matrix[1][3] - projection_view_matrix[1][1];
    planes[FrustrumPlane::FRUSTUM_UP].normal.z		  = projection_view_matrix[2][3] - projection_view_matrix[2][1];
    planes[FrustrumPlane::FRUSTUM_UP].distanceToOrgin   = projection_view_matrix[3][3] - projection_view_matrix[3][1];

    planes[FrustrumPlane::FRUSTUM_FAR].normal.x		   = projection_view_matrix[0][3] - projection_view_matrix[0][2];
    planes[FrustrumPlane::FRUSTUM_FAR].normal.y		   = projection_view_matrix[1][3] - projection_view_matrix[1][2];
    planes[FrustrumPlane::FRUSTUM_FAR].normal.z		   = projection_view_matrix[2][3] - projection_view_matrix[2][2];
    planes[FrustrumPlane::FRUSTUM_FAR].distanceToOrgin   = projection_view_matrix[3][3] - projection_view_matrix[3][2];

    planes[FrustrumPlane::FRUSTUM_NEAR].normal.x		  = projection_view_matrix[0][3] + projection_view_matrix[0][2];
    planes[FrustrumPlane::FRUSTUM_NEAR].normal.y		  = projection_view_matrix[1][3] + projection_view_matrix[1][2];
    planes[FrustrumPlane::FRUSTUM_NEAR].normal.z		  = projection_view_matrix[2][3] + projection_view_matrix[2][2];
    planes[FrustrumPlane::FRUSTUM_NEAR].distanceToOrgin = projection_view_matrix[3][3] + projection_view_matrix[3][2];

    // Normalize all planeslane normals
	for (int i = 0; i < 6; i++)
        planes[i].Normalize();
}

bool CFrustrum::SphereIntersection(const glm::vec3& center, const float& radius)
{	
	for (int i = 0; i < 6; i++)
	{
		// Plane-sphere intersection test. If p*n + d + r < 0 then we're outside the plane.
        if (glm::dot(center, planes[i].normal) + planes[i].distanceToOrgin + radius <= 0)
			return false;
	}
	return true;
}
