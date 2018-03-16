#include "ComponentFactory.h"
#include "AbstractComponent.h"
#include "Animation/Animator.h"
#include "Renderer/RendererComponent.hpp"

namespace GameEngine
{
	namespace Components
	{
		ComponentFactory::ComponentFactory(ComponentController& componentController, Time& time, CResourceManager& resourcesManager)
			: componentController_(componentController)
			, time_(time)
			, resourcesManager_(resourcesManager)
		{
		}
		std::unique_ptr<AbstractComponent> ComponentFactory::Create(ComponentsType type)
		{
			switch (type)
			{
			case ComponentsType::Animator:
			{
				return CreateAndBasicInitialize<Animator>();
			}
			case ComponentsType::Renderer:
			{
				return CreateAndBasicInitialize<RendererComponent>();
			}
			}
			return nullptr;
		}
		void ComponentFactory::SetRendererManager(Renderer::RenderersManager * rendererManager)
		{
			rendererManager_ = rendererManager;
		}
		template<class T>
		std::unique_ptr<T> ComponentFactory::CreateAndBasicInitialize()
		{
			auto comp = std::make_unique<T>();
			comp->SetComponentController(&componentController_);
			comp->SetTime(&time_);
			comp->SetResourceManager(&resourcesManager_);
			comp->SetRendererManager(rendererManager_);
			comp->ReqisterFunctions();
			return comp;
		}
	} // Components
} // GameEngine
