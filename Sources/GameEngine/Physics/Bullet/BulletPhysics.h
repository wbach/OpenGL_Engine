#pragma once
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
	namespace Physics
	{
		class BulletPhysics : public IPhysicsApi
		{
		public:
			BulletPhysics();
			virtual void Simulate() override;
			virtual void SetSimulationStep(float step) override;
			virtual void EnableSimulation() override;
			virtual void DisableSimulation() override;
			virtual uint32 CreateBoxColider(const vec3& positionOffset, const vec3& size) override;
			virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius) override;
			//virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
			virtual uint32 CreateTerrainColider(const vec3& positionOffset, const vec2ui& size, std::vector<float>& data, float hightFactor) override;
			virtual uint32 CreateRigidbody(uint32 shapeId, common::Transform&, float mass, bool isStatic) override;
			virtual void SetVelocityRigidbody(uint32 rigidBodyId,  const vec3& velocity) override;
			virtual void RemoveRigidBody(uint32 id) override;

		private:
			struct Pimpl;
			std::unique_ptr<Pimpl> impl_;
			float simulationStep_;
			bool simualtePhysics_;
			uint32 id_;
		};
	}

} // GameEngine
