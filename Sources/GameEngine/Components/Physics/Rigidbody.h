#pragma once
#include "GameEngine/Components/AbstractComponent.h"

namespace GameEngine
{
	namespace Components
	{
		class Rigidbody : public AbstractComponent
		{
		public:
			Rigidbody();
			void OnAwake();
			virtual void ReqisterFunctions() override;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
