#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
	namespace Components
	{
		class TerrainShape : public CollisionShape
		{
		public:
			TerrainShape();
			void Update();
			virtual void ReqisterFunctions() override;
			void OnAwake();

		public:
			void SetSize(const vec2ui& size);
			void SetHeightFactor(float factor);
			void SetData(std::vector<float>* data);

		private:
			vec2ui size_;
			float heightFactor_;
			std::vector<float>* data_;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
