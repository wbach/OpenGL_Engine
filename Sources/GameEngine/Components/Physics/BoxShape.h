#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
	namespace Components
	{
		class BoxShape : public CollisionShape
		{
		public:
			BoxShape();
			void Update();
			virtual void ReqisterFunctions() override;
			void OnAwake();

		public:
			void SetSize(const vec3& size);
			void SetSize(float size);
		
		private:
			vec3 size_;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
