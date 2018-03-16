#pragma once
#include "ComponentsTypes.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include <memory>

class CResourceManager;

namespace GameEngine
{
	namespace Renderer
	{
		class RenderersManager;
	}

	namespace Components
	{
		class AbstractComponent;

		class ComponentFactory
		{
		public:
			ComponentFactory(ComponentController& componentController, Time& time, CResourceManager& resourcesManager);
			std::unique_ptr<AbstractComponent> Create(ComponentsType type);
			void SetRendererManager(Renderer::RenderersManager* rendererManager);

		private:
			template<class T>
			std::unique_ptr<T> CreateAndBasicInitialize();

		private:
			ComponentController& componentController_;
			CResourceManager&	resourcesManager_;
			Renderer::RenderersManager* rendererManager_;
			Time& time_;
		};
	} // Components
} // GameEngine
