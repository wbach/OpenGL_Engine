#pragma once
#include "GameEngine/Components/AbstractComponent.h"
#include "GameEngine/Physics/ShapeType.h"

namespace GameEngine
{
	namespace Components
	{
		class CollisionShape;

		class Rigidbody : public AbstractComponent
		{
		public:
			Rigidbody();
			~Rigidbody();
			void OnStart();
			virtual void ReqisterFunctions() override;

		public:
			void SetMass(float mass);
			void SetIsStatic(bool is);
			void SetCollisionShape(CollisionShape* collisionShape);
			void SetVelocity(const vec3& velocity);

		private:
			float mass_;
			bool isStatic_;
			CollisionShape* collisionShape_;
			uint32 rigidBodyId;

		public:
			static ComponentsType type;
			ShapeType shapeType_;
		};
	} // Components
} // GameEngine
