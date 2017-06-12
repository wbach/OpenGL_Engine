#pragma once
#include <glm/glm.hpp>
#include "../Utils/Types.h"

class CTransform
{
public:
	CTransform();
	CTransform(const glm::vec2& pos);
	CTransform(const glm::vec3& pos);
	CTransform(const glm::vec3& pos, const glm::vec3& rot);
	CTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
	
	void IncrasePosition(float dx, float dy, float dz, uint32 index = 0);
	void IncrasePosition(glm::vec3 v, uint32 index = 0);
	void IncreaseRotation(float dx, float dy, float dz);
	void UpdateMatrix();
	
	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;
	glm::vec2 GetPositionXZ() const;
	const glm::mat4& GetMatrix() const;

	void SetNormalizedSize(const glm::vec3& size);
	void SetScale(const glm::vec3& s);
	void SetPosition(const glm::vec3& pos);

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 matrix;
	glm::vec3 normalized = glm::vec3(1.f);
};
