#pragma once
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "ComponentsTypes.h"
#include <memory>

class CGameObject;

namespace GameEngine
{
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
			void SetComponentController(ComponentController* componentController);
			inline ComponentsType GetType();

		protected:
			inline void RegisterFunction(FunctionType, std::function<void()> func);

		protected:
			CGameObject* thisObject = nullptr;
			Time* time_ = nullptr;

		private:
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
		ComponentsType AbstractComponent::GetType()
		{
			return type_;
		}
	} // Components
} // GameEngine