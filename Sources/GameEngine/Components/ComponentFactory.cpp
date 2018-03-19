#include "ComponentFactory.h"
#include "AbstractComponent.h"
#include "Animation/Animator.h"
#include "Renderer/RendererComponent.hpp"

namespace GameEngine
{
	namespace Components
	{
		ComponentFactory::ComponentFactory(ComponentController& componentController, Time& time, std::shared_ptr<CResourceManager>& resourceManager)
			: componentController_(componentController)
			, time_(time)
			, resourceManager_(resourceManager)
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
			comp->SetResourceManager(resourceManager_.get());
			comp->SetRendererManager(rendererManager_);
			comp->ReqisterFunctions();
			return comp;
		}
	} // Components
} // GameEngine
