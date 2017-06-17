#include "Transform.h"
#include "GLM/GLMUtils.h"

CTransform::CTransform() :CTransform(vec3(0)) {}

CTransform::CTransform(const vec2 & pos) : CTransform(vec3(pos.x, 0, pos.y)) {}

CTransform::CTransform(const vec3 & pos) : CTransform(pos, vec3(0)) {}

CTransform::CTransform(const vec3 & pos, const vec3 & rot) : CTransform(pos, rot, vec3(1)) {}
CTransform::CTransform(const vec3 & pos, const vec3 & rot, const vec3 & scale) : position(pos), rotation(rot), scale(scale), matrix(Utils::CreateTransformationMatrix(pos, rot, scale)) {}

void CTransform::IncrasePosition(float dx, float dy, float dz, uint32 index)
{
	IncrasePosition(vec3(dx, dy, dz));
}

void CTransform::IncrasePosition(vec3 v, uint32 index)
{
	position += v;
	UpdateMatrix();
}

void CTransform::IncreaseRotation(float dx, float dy, float dz)
{
	rotation += vec3(dx, dy, dz);
	UpdateMatrix();
}

void CTransform::UpdateMatrix()
{
	matrix = Utils::CreateTransformationMatrix(position, rotation, scale * normalized);
}

vec3 CTransform::GetPosition() const { return position; }

vec3 CTransform::GetRotation() const { return rotation; }

vec2 CTransform::GetPositionXZ() const { return vec2(position.x, position.z); }

const mat4 & CTransform::GetMatrix() const { return matrix; }

void CTransform::SetNormalizedSize(const vec3 & size)
{
	normalized = size;
	UpdateMatrix();
}

void CTransform::SetScale(const vec3 & s)
{
	scale = s;
	UpdateMatrix();
}

void CTransform::SetPosition(const vec3 & pos) { position = pos;  UpdateMatrix(); }
