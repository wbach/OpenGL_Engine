#include "Transform.h"
#include "GLM/GLMUtils.h"
#include <limits>

namespace common
{
	Transform::Transform() :Transform(vec3(0)){}
	Transform::Transform(const vec2 & pos) : Transform(vec3(pos.x, 0, pos.y)){}
	Transform::Transform(const vec3 & pos) : Transform(pos, vec3(0)){}
	Transform::Transform(const vec3 & pos, const vec3 & rot) : Transform(pos, rot, vec3(1)){}
	Transform::Transform(const vec3 & pos, const vec3 & rot, const vec3 & scale) 
		: isDynamic_(false)
		, context_({ pos, rot, scale})
		, snapshoot_({ pos, rot, scale })
		, matrix(Utils::CreateTransformationMatrix(pos, rot, scale))
	{
	}

	Transform::Transform(const Transform & t)
		: isDynamic_(t.isDynamic_)
		, context_(t.context_)
		, snapshoot_(t.snapshoot_)
		, matrix(t.matrix)
	{
	}

	void Transform::IncrasePosition(float dx, float dy, float dz, uint32 index)
	{
		IncrasePosition(vec3(dx, dy, dz));
	}

	void Transform::IncrasePosition(vec3 v, uint32 index)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.position += v;
	}

	void Transform::IncreaseRotation(float dx, float dy, float dz)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.rotation += vec3(dx, dy, dz);
	}

	void Transform::SetScale(float s)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.scale = vec3(s);
	}

	void Transform::SetScale(const vec3& s)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.scale = s;
	}

	void Transform::SetPosition(const vec3 & pos)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.position = pos;
	}

    void Transform::SetYPosition(float pos)
    {
        std::lock_guard<std::mutex> l(contextMutex_);
        context_.position.y = pos;
    }

	void Transform::SetPositionXZ(const vec2 & pos)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.position.x = pos.x;
		context_.position.z = pos.y;
	}

	void Transform::SetRotation(const vec3 & r)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		context_.rotation = r;
	}

	void Transform::SetRotate(Axis axis, float v)
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		switch (axis)
		{
		case X: context_.rotation.x = v; break;
		case Y: context_.rotation.y = v; break;
		case Z: context_.rotation.z = v; break;
		}
	}

	void Transform::TakeSnapShoot()
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		snapshoot_ = context_;
		UpdateMatrix();
	}

	void Transform::UpdateMatrix()
	{
		matrix = Utils::CreateTransformationMatrix(context_.position, context_.rotation, context_.scale);
	}

	const TransformContext& Transform::GetSnapShoot() const
	{
		return snapshoot_;
	}

	vec3 Transform::GetPosition()
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		return context_.position;
	}

	vec3 Transform::GetRotation()
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		return context_.rotation;
	}

	vec3 Transform::GetScale()
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		return context_.scale;
	}

	vec2 Transform::GetPositionXZ()
	{
		std::lock_guard<std::mutex> l(contextMutex_);
		return vec2(context_.position.x, context_.position.z);
	}

	const mat4& Transform::GetMatrix() const
	{
		return matrix;
	}
} // common