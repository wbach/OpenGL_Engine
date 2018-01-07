#include "Transform.h"
#include "GLM/GLMUtils.h"

CTransform::CTransform() :CTransform(vec3(0)) 
{
}

CTransform::CTransform(const vec2 & pos) : CTransform(vec3(pos.x, 0, pos.y))
{
}

CTransform::CTransform(const vec3 & pos) : CTransform(pos, vec3(0))
{
}

CTransform::CTransform(const vec3 & pos, const vec3 & rot) : CTransform(pos, rot, vec3(1))
{
}

CTransform::CTransform(const vec3 & pos, const vec3 & rot, const vec3 & scale) 
	: position(pos)
	, rotation(rot)
	, scale(scale)
	, matrix(Utils::CreateTransformationMatrix(pos, rot, scale))
{
}

void CTransform::IncrasePosition(float dx, float dy, float dz, uint32 index)
{
	IncrasePosition(vec3(dx, dy, dz));
}

void CTransform::IncrasePosition(vec3 v, uint32 index)
{
	std::lock_guard<std::mutex> l(pmutex);
	position += v;
	UpdateMatrix();
}

void CTransform::IncreaseRotation(float dx, float dy, float dz)
{
	std::lock_guard<std::mutex> l(rmutex);
	rotation += vec3(dx, dy, dz);
	UpdateMatrix();
}

void CTransform::UpdateMatrix()
{
	std::lock_guard<std::mutex> l(mmutex);
	matrix = Utils::CreateTransformationMatrix(position, rotation, scale * normalized);
}

const vec3& CTransform::GetPosition()
{ 
	std::lock_guard<std::mutex> l(pmutex); 
	return position; 
}

const vec3& CTransform::GetRotation()
{ 
	std::lock_guard<std::mutex> l(rmutex);
	return rotation; 
}

vec2 CTransform::GetPositionXZ() 
{ 
	std::lock_guard<std::mutex> l(pmutex);
	return vec2(position.x, position.z);
}

const mat4 & CTransform::GetMatrix()
{ 
	std::lock_guard<std::mutex> l(mmutex);
	return matrix;
}

void CTransform::SetNormalizedSize(const vec3 & size)
{
	normalized = size;
	UpdateMatrix();
}

void CTransform::SetScale(const vec3 & s)
{
	std::lock_guard<std::mutex> l(smutex);
	scale = s;
	UpdateMatrix();
}

void CTransform::SetPosition(const vec3 & pos)
{ 
	std::lock_guard<std::mutex> l(pmutex);
	position = pos;  
	UpdateMatrix();
}
