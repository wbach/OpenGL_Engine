#pragma once
#include "CollisionShape.h"

class CModel;

namespace GameEngine
{
	namespace Components
	{
		class MeshShape : public CollisionShape
		{
		public:
			MeshShape();
			void Update();
			virtual void ReqisterFunctions() override;
			void OnAwake();

		public:
			void SetModel(CModel* model);

		private:
			vec3 size_;
			CModel* model_;

		public:
			static ComponentsType type;
		};
	} // Components
} // GameEngine
