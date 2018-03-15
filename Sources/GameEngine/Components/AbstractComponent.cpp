#include "AbstractComponent.h"

namespace GameEngine
{
	namespace Components
	{
		void AbstractComponent::SetComponentController(ComponentController* componentController)
		{
			componentController_ = componentController;
		}
		AbstractComponent::AbstractComponent(ComponentsType type)
			: type_(type)
		{
		}
		AbstractComponent::~AbstractComponent()
		{
			if (componentController_ == nullptr)
				return;

			for (auto id : ids_)
				componentController_->UnRegisterFunction(id.second, id.first);
		}
		void AbstractComponent::SetTime(Time * time)
		{
			time_ = time;
		}
		void AbstractComponent::SetObjectPtr(CGameObject* ptr)
		{
			thisObject = ptr;
		}
	} // Components
} // GameEngine