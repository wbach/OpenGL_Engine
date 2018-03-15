#pragma once
#include "ComponentsTypes.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include <memory>

namespace GameEngine
{
	namespace Components
	{
		class AbstractComponent;

		class ComponentFactory
		{
		public:
			ComponentFactory(ComponentController& componentController, Time& time);
			std::unique_ptr<AbstractComponent> Create(ComponentsType type);

		private:
			ComponentController& componentController_;
			Time& time_;
		};
	} // Components
} // GameEngine
