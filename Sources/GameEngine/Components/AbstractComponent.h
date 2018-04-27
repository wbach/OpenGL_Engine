#pragma once
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "ComponentsTypes.h"
#include <memory>

class CGameObject;
class CResourceManager;

namespace GameEngine
{
	struct ICamera;

	namespace Renderer
	{
		class RenderersManager;
	} // Renderer

	namespace Components
	{
		class AbstractComponent
		{
		public:
			AbstractComponent(ComponentsType type);
			virtual ~AbstractComponent();
			virtual void ReqisterFunctions() {};
			void SetTime(Time* time);
			void SetObjectPtr(CGameObject* ptr);
			void SetResourceManager(CResourceManager* manager);
			void SetRendererManager(Renderer::RenderersManager* manager);
			void SetComponentController(ComponentController* componentController);
			void SetCamera(std::shared_ptr<ICamera>* camera);
			inline ComponentsType GetType();

		protected:
			inline void RegisterFunction(FunctionType, std::function<void()> func);
			inline ICamera* GetCamera();

		protected:
			Time* time_ = nullptr;
			CGameObject* thisObject = nullptr;
			CResourceManager* resourceManager_ = nullptr;
			Renderer::RenderersManager* renderersManager_ = nullptr;

		private:
			std::shared_ptr<ICamera>* camera_ = nullptr;
			ComponentController* componentController_ = nullptr;
			std::unordered_map<uint32, FunctionType> ids_;
			ComponentsType type_;
		};

		typedef std::unique_ptr<AbstractComponent> AbstractComponentPtr;

		void AbstractComponent::RegisterFunction(FunctionType type, std::function<void()> func)
		{
			if (componentController_ == nullptr)
				return;

			ids_[componentController_->RegisterFunction(type, func)] = type;
		}
		inline ICamera* AbstractComponent::GetCamera()
		{
			return camera_ ? camera_->get() : nullptr;
		}
		ComponentsType AbstractComponent::GetType()
		{
			return type_;
		}
	} // Components
} // GameEngine