#include "BoxShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType BoxShape::type = ComponentsType::BoxShape;

		BoxShape::BoxShape()
			: CollisionShape(ComponentsType::BoxShape)
			, size_(1.f)
		{
		}
		void BoxShape::Update()
		{
		}
		void BoxShape::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&BoxShape::OnAwake, this));
		}
		void BoxShape::OnAwake()
		{
			collisionShapeId_ = physicsApi_->CreateBoxColider(positionOffset_, size_);
		}
		void BoxShape::SetSize(const vec3 & size)
		{
			size_ = size;
		}
		void BoxShape::SetSize(float size)
		{
			size_ = vec3(size);
		}
	} // Components
} // GameEngine
