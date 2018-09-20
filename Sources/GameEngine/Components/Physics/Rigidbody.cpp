#include "Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType Rigidbody::type = ComponentsType::Rigidbody;

		Rigidbody::Rigidbody()
			:AbstractComponent(ComponentsType::Rigidbody)
		{
		}
		void Rigidbody::OnAwake()
		{
			common::Transform transform;
			physicsApi_->CreateRigidbody(transform, 0, false);
		}
		void Rigidbody::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&Rigidbody::OnAwake, this));
		}
	} // Components
} // GameEngine