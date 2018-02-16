#pragma once
#include "Types.h"
#include "Mutex.hpp"
#include <unordered_map>
#include <functional>

namespace common
{
	enum Axis
	{
		X = 0,
		Y,
		Z
	};
	
	typedef std::function<void(const vec3& pos, const vec3& rot, const vec3& scale, const mat4&)> TransformChangeSubscribers;

	class Transform
	{
	public:
		Transform();
		Transform(const vec2& pos);
		Transform(const vec3& pos);
		Transform(const vec3& pos, const vec3& rot);
		Transform(const vec3& pos, const vec3& rot, const vec3& scale);
		Transform(const Transform& transform);

		void SubscribeOnChange(const std::string& label, TransformChangeSubscribers subscriber);
		void UnsubscribeOnChange(const std::string& label);

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
		void SetPositionXZ(const vec2& pos);
		void SetRotation(const vec3& r);
		void SetRotate(Axis axis, float v);

	private:
		std::unordered_map<std::string, TransformChangeSubscribers> transformChangeSubscribers_;
		vec3 position;	std::mutex pmutex;
		vec3 rotation;	std::mutex rmutex;
		vec3 scale;		std::mutex smutex;
		mat4 matrix;	std::mutex mmutex;
		vec3 normalized = vec3(1.f); 
	};
} // common
