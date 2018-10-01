#include "SphereShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType SphereShape::type = ComponentsType::SphereShape;

		SphereShape::SphereShape()
			: CollisionShape(ComponentsType::SphereShape)
			, size_(1.f)
		{
		}
		void SphereShape::Update()
		{
		}
		void SphereShape::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&SphereShape::OnAwake, this));
		}
		void SphereShape::OnAwake()
		{
			collisionShapeId_ = physicsApi_->CreateSphereColider(positionOffset_, size_);
		}
		void SphereShape::SetSize(float size)
		{
			size_ = size;
		}
	} // Components
} // GameEngine
