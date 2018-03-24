#pragma once
#include "GameEngine/Components/AbstractComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
	namespace Components
	{
		class TreeRendererComponent : public AbstractComponent
		{
		public:
			TreeRendererComponent();
			void SetTopModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
			void SetBottomModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
			void SetPositions(const std::vector<vec3>& positions);
			virtual void ReqisterFunctions() override;
			inline ModelWrapper& GetTopModelWrapper();
			inline ModelWrapper& GetBottomModelWrapper();
			inline std::vector<vec3>& GetPositions();

		private:
			void Subscribe();
			void UnSubscribe();

		private:
			ModelWrapper top_;
			ModelWrapper bottom_;
			std::vector<vec3> positions_;

		public:
			static ComponentsType type;
		};

		ModelWrapper& TreeRendererComponent::GetTopModelWrapper()
		{
			return top_;
		}
		ModelWrapper& TreeRendererComponent::GetBottomModelWrapper()
		{
			return bottom_;
		}
		std::vector<vec3>& TreeRendererComponent::GetPositions()
		{
			return positions_;
		}
	} // Components
} // GameEngine
