#include "BoxCollider.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType BoxCollider::type = ComponentsType::BoxCollider;

		BoxCollider::BoxCollider()
			: AbstractComponent(ComponentsType::BoxCollider)
		{
		}
		void BoxCollider::Update()
		{
		}
		void BoxCollider::ReqisterFunctions()
		{
			auto rigidbody = thisObject->GetComponent<Rigidbody>();

			if (rigidbody == nullptr)
			{
				return;
			}
		}
	} // Components
} // GameEngine
