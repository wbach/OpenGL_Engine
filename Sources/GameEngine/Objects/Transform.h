#pragma once
#include "Types.h"
#include <mutex>

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
	
	const vec3& GetPosition();
	const vec3& GetRotation();
	vec2 GetPositionXZ();
	const mat4& GetMatrix();

	void SetNormalizedSize(const vec3& size);
	void SetScale(const vec3& s);
	void SetPosition(const vec3& pos);

private:
	vec3 position;	std::mutex pmutex;
	vec3 rotation;	std::mutex rmutex;
	vec3 scale;		std::mutex smutex;
	mat4 matrix;	std::mutex mmutex;
	vec3 normalized = vec3(1.f); 
};
