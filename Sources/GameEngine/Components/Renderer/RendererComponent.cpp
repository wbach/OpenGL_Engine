#include "RendererComponent.hpp"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType RendererComponent::type = ComponentsType::Renderer;

		RendererComponent::RendererComponent()
		: AbstractComponent(ComponentsType::Renderer)
		{
		}

		RendererComponent::~RendererComponent()
		{
			if (renderersManager_ == nullptr)
				return;

			renderersManager_->UnSubscribe(thisObject);
		}

		void RendererComponent::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&RendererComponent::Subscribe, this));
		}
		void RendererComponent::AddModel(const std::string& filename, GameEngine::LevelOfDetail i)
		{
			if (filename.empty() || resourceManager_ == nullptr)
				return;

			auto model = GameEngine::LoadModel(resourceManager_, filename);
			model_.Add(model, i);
		}
		void RendererComponent::Subscribe()
		{
			if (renderersManager_ == nullptr)
				return;

			renderersManager_->Subscribe(thisObject);
		}
		void RendererComponent::UnSubscribe()
		{
			if (renderersManager_ == nullptr)
				return;

			renderersManager_->UnSubscribe(thisObject);
		}
	} // Components
} // GameEngine