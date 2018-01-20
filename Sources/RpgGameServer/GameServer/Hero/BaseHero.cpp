#include "BaseHero.h"

namespace GameServer
{
	namespace Hero
	{
		void BaseHero::UpdateAllControllers(float dt)
		{
			for (auto& controller : controllers_)
				controller.second->Update(dt);
		}
		void BaseHero::AddController(Controllers::IControllerPtr cotroller)
		{
			controllers_[cotroller->GetType()] = cotroller;
		}
		Controllers::IControllerPtr BaseHero::GetControllerByType(Controllers::Types type)
		{
			return controllers_[type];
		}
	} // Hero
} // GameServer