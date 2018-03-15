#include "ComponentFactory.h"
#include "AbstractComponent.h"
#include "Animation/Animator.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentFactory::ComponentFactory(ComponentController& componentController, Time& time)
			: componentController_(componentController)
			, time_(time)
		{
		}
		std::unique_ptr<AbstractComponent> ComponentFactory::Create(ComponentsType type)
		{
			switch (type)
			{
			case ComponentsType::Animator:
			{
				auto anim = std::make_unique<Animator>();
				anim->SetComponentController(&componentController_);
				anim->SetTime(&time_);
				anim->ReqisterFunctions();
				return anim;
			}
			}
			return nullptr;
		}
	} // Components
} // GameEngine
