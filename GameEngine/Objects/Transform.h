#pragma once
#include "Types.h"

class CTransform
{
public:
	CTransform();
	CTransform(const vec2& pos);
	CTransform(const vec3& pos);
	CTransform(const vec3& pos, const vec3& rot);
	CTransform(const vec3& pos, const vec3& rot, const vec3& scale);
	
	void IncrasePosition(float dx, float dy, float dz, uint32 index = 0);
	void IncrasePosition(vec3 v, uint32 index = 0);
	void IncreaseRotation(float dx, float dy, float dz);
	void UpdateMatrix();
	
	vec3 GetPosition() const;
	vec3 GetRotation() const;
	vec2 GetPositionXZ() const;
	const mat4& GetMatrix() const;

	void SetNormalizedSize(const vec3& size);
	void SetScale(const vec3& s);
	void SetPosition(const vec3& pos);

private:
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4 matrix;
	vec3 normalized = vec3(1.f);
};
