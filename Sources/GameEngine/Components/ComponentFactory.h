#pragma once
#include "ComponentsTypes.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include <memory>

class CResourceManager;

namespace GameEngine
{
	struct ICamera;

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
			ComponentFactory(ComponentController& componentController, Time& time, std::shared_ptr<CResourceManager>& resourceManager, std::shared_ptr<ICamera>& camera);
			std::unique_ptr<AbstractComponent> Create(ComponentsType type);
			void SetRendererManager(Renderer::RenderersManager* rendererManager);

		private:
			template<class T>
			std::unique_ptr<T> CreateAndBasicInitialize();

		private:
			ComponentController& componentController_;
			std::shared_ptr<CResourceManager>& resourceManager_;
			Renderer::RenderersManager* rendererManager_;
			std::shared_ptr<ICamera>& camera_;
			Time& time_;
		};
	} // Components
} // GameEngine
