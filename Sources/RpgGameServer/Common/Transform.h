#pragma once
#include "Types.h"
#include "Mutex.hpp"

namespace common
{
	enum Axis
	{
		X = 0,
		Y,
		Z
	};
	
	class Transform
	{
	public:
		Transform();
		Transform(const vec2& pos);
		Transform(const vec3& pos);
		Transform(const vec3& pos, const vec3& rot);
		Transform(const vec3& pos, const vec3& rot, const vec3& scale);
		
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
		void SetRotate(Axis axis, float v);

	private:
		vec3 position;	std::mutex pmutex;
		vec3 rotation;	std::mutex rmutex;
		vec3 scale;		std::mutex smutex;
		mat4 matrix;	std::mutex mmutex;
		vec3 normalized = vec3(1.f); 
	};
} // common
