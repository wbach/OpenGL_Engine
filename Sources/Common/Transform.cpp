#include "Transform.h"
#include "GLM/GLMUtils.h"

namespace common
{
	Transform::Transform() :Transform(vec3(0)){}
	Transform::Transform(const vec2 & pos) : Transform(vec3(pos.x, 0, pos.y)){}
	Transform::Transform(const vec3 & pos) : Transform(pos, vec3(0)){}
	Transform::Transform(const vec3 & pos, const vec3 & rot) : Transform(pos, rot, vec3(1)){}
	Transform::Transform(const vec3 & pos, const vec3 & rot, const vec3 & scale) 
		: position(pos)
		, rotation(rot)
		, scale(scale)
		, matrixChanged(true)
		, matrix(Utils::CreateTransformationMatrix(pos, rot, scale))
	{
	}

	Transform::Transform(const Transform & t)
		: position(t.position)
		, rotation(t.rotation)
		, scale(t.scale)
		, matrixChanged(true)
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

		TransformChanged();
	}

	void Transform::IncreaseRotation(float dx, float dy, float dz)
	{
		std::lock_guard<std::mutex> l(rmutex);
		rotation += vec3(dx, dy, dz);

		TransformChanged();
	}

	void Transform::SetScale(float s)
	{
		std::lock_guard<std::mutex> l(smutex);
		scale = vec3(s);
		TransformChanged();
	}

	void Transform::SetScale(const vec3 & s)
	{
		std::lock_guard<std::mutex> l(smutex);
		scale = s;
		TransformChanged();
	}

	void Transform::SetPosition(const vec3 & pos)
	{
		std::lock_guard<std::mutex> l(pmutex);
		position = pos;
		TransformChanged();
	}

	void Transform::SetPositionXZ(const vec2 & pos)
	{
		std::lock_guard<std::mutex> l(pmutex);
		position.x = pos.x;
		position.z = pos.y;
		TransformChanged();
	}

	void Transform::SetRotation(const vec3 & r)
	{
		std::lock_guard<std::mutex> l(rmutex);
		rotation = r;
		TransformChanged();
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
		TransformChanged();
	}

	void Transform::TransformChanged()
	{
		matrixChanged.store(true);

		for (auto& sub : transformChangeSubscribers_)
			sub.second(position, rotation, scale);
	}

	void Transform::UpdateMatrix()
	{
		matrix = Utils::CreateTransformationMatrix(GetPosition(), GetRotation(), GetScale());
		matrixChanged.store(false);	
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

	vec3 Transform::GetPosition()
	{
		std::lock_guard<std::mutex> l(pmutex);
		return position;
	}

	vec3 Transform::GetRotation()
	{
		std::lock_guard<std::mutex> l(rmutex);
		return rotation;
	}

	vec3 Transform::GetScale()
	{
		std::lock_guard<std::mutex> l(smutex);
		return scale;
	}

	vec2 Transform::GetPositionXZ()
	{
		std::lock_guard<std::mutex> l(pmutex);
		return vec2(position.x, position.z);
	}

	const mat4& Transform::GetMatrix()
	{
		if(matrixChanged.load())
			UpdateMatrix();
		return matrix;
	}
} // common