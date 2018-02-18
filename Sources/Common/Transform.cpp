#include "Transform.h"
#include "GLM/GLMUtils.h"

namespace common
{
	Transform::Transform() :Transform(vec3(0)) 
	{
	}

	Transform::Transform(const vec2 & pos) : Transform(vec3(pos.x, 0, pos.y))
	{
	}

	Transform::Transform(const vec3 & pos) : Transform(pos, vec3(0))
	{
	}

	Transform::Transform(const vec3 & pos, const vec3 & rot) : Transform(pos, rot, vec3(1))
	{
	}

	Transform::Transform(const vec3 & pos, const vec3 & rot, const vec3 & scale) 
		: position(pos)
		, rotation(rot)
		, scale(scale)
		, matrix(Utils::CreateTransformationMatrix(pos, rot, scale))
	{
	}

	Transform::Transform(const Transform & t)
		: position(t.position)
		, rotation(t.rotation)
		, scale(t.scale)
		, matrix(t.matrix)
	{
	}

	void Transform::IncrasePosition(float dx, float dy, float dz, uint32 index)
	{
		IncrasePosition(vec3(dx, dy, dz));
	}

	void Transform::IncrasePosition(vec3 v, uint32 index)
	{
		std::lock_guard<std::mutex> l(pmutex);
		position += v;
		UpdateMatrix();
	}

	void Transform::IncreaseRotation(float dx, float dy, float dz)
	{
		std::lock_guard<std::mutex> l(rmutex);
		rotation += vec3(dx, dy, dz);
		UpdateMatrix();
	}

	void Transform::UpdateMatrix()
	{
		std::lock_guard<std::mutex> l(mmutex);
		matrix = Utils::CreateTransformationMatrix(position, rotation, scale );

		for (auto& sub : transformChangeSubscribers_)
			sub.second(position, rotation, scale, matrix);
	}

	void Transform::SubscribeOnChange(const std::string& label, TransformChangeSubscribers subscriber)
	{
		transformChangeSubscribers_[label] = subscriber;
	}

	void Transform::UnsubscribeOnChange(const std::string & label)
	{
		if (transformChangeSubscribers_.count(label) != 0)
			transformChangeSubscribers_.erase(label);
	}

	const vec3& Transform::GetPosition()
	{ 
		std::lock_guard<std::mutex> l(pmutex); 
		return position; 
	}

	const vec3& Transform::GetRotation()
	{ 
		std::lock_guard<std::mutex> l(rmutex);
		return rotation; 
	}

	vec2 Transform::GetPositionXZ() 
	{ 
		std::lock_guard<std::mutex> l(pmutex);
		return vec2(position.x, position.z);
	}

	const mat4 & Transform::GetMatrix()
	{ 
		std::lock_guard<std::mutex> l(mmutex);
		return matrix;
	}

	void Transform::SetScale(float s)
	{
		std::lock_guard<std::mutex> l(smutex);
		scale = vec3(s);
		UpdateMatrix();
	}

	void Transform::SetScale(const vec3 & s)
	{
		std::lock_guard<std::mutex> l(smutex);
		scale = s;
		UpdateMatrix();
	}

	void Transform::SetPosition(const vec3 & pos)
	{ 
		std::lock_guard<std::mutex> l(pmutex);
		position = pos;  
		UpdateMatrix();
	}

	void Transform::SetPositionXZ(const vec2 & pos)
	{
		std::lock_guard<std::mutex> l(pmutex);
		position.x = pos.x;
		position.z = pos.y;
		UpdateMatrix();
	}

	void Transform::SetRotation(const vec3 & r)
	{
		std::lock_guard<std::mutex> l(rmutex);
		rotation = r;
		UpdateMatrix();
	}

	void Transform::SetRotate(Axis axis, float v)
	{
		std::lock_guard<std::mutex> l(rmutex);
		switch (axis)
		{
		case X: rotation.x = v; break;
		case Y: rotation.y = v; break;
		case Z: rotation.z = v; break;
		}
		
		UpdateMatrix();
	}
} // common