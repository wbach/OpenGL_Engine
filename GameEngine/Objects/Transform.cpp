#include "Transform.h"
#include "../Utils/GLM/GLMUtils.h"

CTransform::CTransform() :CTransform(glm::vec3(0)) {}

CTransform::CTransform(const glm::vec2 & pos) : CTransform(glm::vec3(pos.x, 0, pos.y)) {}

CTransform::CTransform(const glm::vec3 & pos) : CTransform(pos, glm::vec3(0)) {}

CTransform::CTransform(const glm::vec3 & pos, const glm::vec3 & rot) : CTransform(pos, rot, glm::vec3(1)) {}
CTransform::CTransform(const glm::vec3 & pos, const glm::vec3 & rot, const glm::vec3 & scale) : position(pos), rotation(rot), scale(scale), matrix(Utils::CreateTransformationMatrix(pos, rot, scale)) {}

void CTransform::IncrasePosition(float dx, float dy, float dz, uint index)
{
	IncrasePosition(glm::vec3(dx, dy, dz));
}

void CTransform::IncrasePosition(glm::vec3 v, uint index)
{
	position += v;
	UpdateMatrix();
}

void CTransform::IncreaseRotation(float dx, float dy, float dz)
{
	rotation += glm::vec3(dx, dy, dz);
	UpdateMatrix();
}

void CTransform::UpdateMatrix()
{
	matrix = Utils::CreateTransformationMatrix(position, rotation, scale * normalized);
}

glm::vec3 CTransform::GetPosition() const { return position; }

glm::vec3 CTransform::GetRotation() const { return rotation; }

glm::vec2 CTransform::GetPositionXZ() const { return glm::vec2(position.x, position.z); }

const glm::mat4 & CTransform::GetMatrix() const { return matrix; }

void CTransform::SetNormalizedSize(const glm::vec3 & size)
{
	normalized = size;
	UpdateMatrix();
}

void CTransform::SetScale(const glm::vec3 & s)
{
	scale = s;
	UpdateMatrix();
}

void CTransform::SetPosition(const glm::vec3 & pos) { position = pos;  UpdateMatrix(); }
