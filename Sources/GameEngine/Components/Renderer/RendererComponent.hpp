#pragma once
#include "GameEngine/Components/AbstractComponent.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
	namespace Components
	{
		class RendererComponent : public AbstractComponent
		{
		public:
			RendererComponent();
			void AddModel(const std::string& filename, GameEngine::LevelOfDetail i = GameEngine::LevelOfDetail::L1);
			virtual void ReqisterFunctions() override;
			inline ModelWrapper& GetModelWrapper();

		private:
			void Subscribe();
			void UnSubscribe();

		private:
			ModelWrapper model_;

		public:
			static ComponentsType type;
		};

		ModelWrapper& RendererComponent::GetModelWrapper()
		{
			return model_;
		}
	} // Components
} // GameEngine
