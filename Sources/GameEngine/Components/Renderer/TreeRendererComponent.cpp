#include "TreeRendererComponent.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
	namespace Components
	{
		ComponentsType TreeRendererComponent::type = ComponentsType::TreeRenderer;

		TreeRendererComponent::TreeRendererComponent()
			: AbstractComponent(ComponentsType::TreeRenderer)
		{
		}

		void TreeRendererComponent::SetPositions(const std::vector<vec3>& positions)
		{
			positions_ = positions;
		}

		void TreeRendererComponent::ReqisterFunctions()
		{
			RegisterFunction(FunctionType::Awake, std::bind(&TreeRendererComponent::Subscribe, this));
		}
		void TreeRendererComponent::SetBottomModel(const std::string& filename, GameEngine::LevelOfDetail i)
		{
			if (filename.empty() || resourceManager_ == nullptr)
				return;

			auto model = GameEngine::LoadModel(resourceManager_, filename);
			bottom_.Add(model, i);
		}
		void TreeRendererComponent::SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i)
		{
			if (filename.empty() || resourceManager_ == nullptr)
				return;

			auto model = GameEngine::LoadModel(resourceManager_, filename);
			top_.Add(model, i);
		}
		void TreeRendererComponent::Subscribe()
		{
			if (renderersManager_ == nullptr)
				return;

			renderersManager_->Subscribe(thisObject);
		}
		void TreeRendererComponent::UnSubscribe()
		{
			if (renderersManager_ == nullptr)
				return;
		}
	} // Components
} // GameEngine
