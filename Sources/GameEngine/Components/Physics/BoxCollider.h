#pragma once
#include "GameEngine/Components/AbstractComponent.h"

namespace GameEngine
{
	namespace Components
	{
		class BoxCollider : public AbstractComponent
		{
		public:
			BoxCollider();
			void Update();
			virtual void ReqisterFunctions() override;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
