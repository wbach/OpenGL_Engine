#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Types.h"
#include <vector>
#include <iostream>
#include <string>

namespace Utils
{
    //static int s_intmone = -1;
	static glm::vec2 s_vec2_zero(0);
	static glm::vec3 s_vec3_zero(0);
	static glm::mat4 s_mat4_one(1.f);
	

	glm::vec3 ColorLerpRGB(const glm::vec3 & c1, const glm::vec3 & c2, const float & blend);

	glm::vec3 TransformPoint(const glm::vec3 & point, const glm::mat4 & matrix);

	glm::vec3 RGBtoFloat(const glm::vec3 & color);

	glm::vec3 RGBtoFloat(const float & r, const float & g, const float & b);

	void PrintVector(const std::string & text, const glm::vec3 & v);

	void PrintVector(const std::string & text, const glm::vec2 & v);

	void PrintMatrix(const std::string & text, const glm::mat4 & m);

	void PrtintMat4(const glm::mat4 & mat);

	float GetMaxFromVector(const glm::vec3 & vector);

	glm::mat4 CreateTransformationMatrix(const glm::vec3& translation = glm::vec3(.0f), const glm::vec3& rotation = glm::vec3(.0f), const glm::vec3& scale = glm::vec3(1.f));

	glm::mat4 CreateTransformationMatrix(const glm::vec2 & translation, const glm::vec2 & scale, const float& rotation = 0.f);

	void CalculateBoudnigBox(const std::vector<float>& positions, glm::vec3 & bounding_min, glm::vec3 & bounding_max, glm::vec3 & bounding_size, glm::vec3 & bounding_center);

	glm::vec3 CalculateMinimumVector(const glm::vec3 & a, const glm::vec3 & b);

	float BarryCentric(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec2 & pos);

	glm::vec3 BarryCentricVec3(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec2 & pos);

	glm::mat4 CreateOffset();

	glm::vec2 toScreenSpace(glm::vec2 & position, glm::vec2 & window_size);

	std::string ToString(const glm::vec3 & v);

	std::string ToString(const glm::vec2 & v);

	std::string ToString(const glm::mat4 & m);

	//float IntersectTriangle(const glm::vec3 & normal, const glm::vec3 & rayPos, const glm::vec3 & rayDirection, const SFaceVertex & vertexes)
	//{
	//	glm::vec3 v0 = vertexes.v3 - vertexes.v1;
	//	glm::vec3 v1 = vertexes.v2 - vertexes.v1;
	//	float d = -1 * (glm::dot(normal, vertexes.v1));
	//	float t = -(normal.x*rayPos.x + normal.y*rayPos.y + normal.z*rayPos.z + d) / (normal.x*rayDirection.x + normal.y*rayDirection.y + normal.z*rayDirection.z);
	//	glm::vec3 point = rayPos + t*rayDirection;
	//	glm::vec3 v2 = point - vertexes.v1;
	//	float dot00 = glm::dot(v0, v0);
	//	float dot01 = glm::dot(v0, v1);
	//	float dot02 = glm::dot(v0, v2);
	//	float dot11 = glm::dot(v1, v1);
	//	float dot12 = glm::dot(v1, v2);
	//	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	//	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	//	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
	//	if ((u >= 0) && (v >= 0) && (u + v < 1))
	//	{
	//		return t;
	//	}
	//	else return -1;
	//}

	//bool DetectSphereTriangleCollision(const glm::vec3 & sp, const float & r, const SFaceVertex & vertexes, const glm::vec3 & normal, SCollisionInfo & colision_info)
	//{
	//	glm::vec3 n = glm::normalize(normal);

	//	float dist1 = IntersectTriangle(-n, sp, n, vertexes);
	//	float dist2 = IntersectTriangle(n, sp, -n, vertexes);

	//	colision_info.collision = false;
	//	colision_info.normal = n;
	//	colision_info.updateVector = glm::vec3(0);

	//	if (dist2 > 0 && dist2 < r) {
	//		colision_info.normal = n;
	//		colision_info.collision = true;
	//		colision_info.t = dist2;
	//		colision_info.updateVector = n*(r - dist2);
	//	}
	//	else if (dist1 > 0 && dist1 < r) {
	//		colision_info.normal = -n;
	//		colision_info.updateVector = -n*(r - dist1);
	//		colision_info.collision = true;
	//		colision_info.t = dist1;
	//		colision_info.collisionY = sp.y + colision_info.updateVector.y;
	//	}
	//	return colision_info.collision;
	//}

}
