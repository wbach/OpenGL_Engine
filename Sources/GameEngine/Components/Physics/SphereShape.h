#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
	namespace Components
	{
		class SphereShape : public CollisionShape
		{
		public:
			SphereShape();
			void Update();
			virtual void ReqisterFunctions() override;
			void OnAwake();

		public:
			void SetSize(float size);

		private:
			float size_;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
