#pragma once
#include "Types.h"
#include "Common/Transform.h"
#include <vector>

namespace GameEngine
{
	namespace Physics
	{
		struct IPhysicsApi
		{
			virtual ~IPhysicsApi() {}
			virtual void Simulate() = 0;
			virtual void EnableSimulation() = 0;
			virtual void DisableSimulation() = 0;
			virtual void SetSimulationStep(float step) = 0;
			virtual uint32 CreateBoxColider(const vec3& positionOffset, float size) = 0;
			virtual uint32 CreateSphereColider(const vec3& positionOffset, float radius) = 0;
			//virtual void CreateMeshColider(vec3 positionOffset, std::vect) = 0;
			virtual uint32 CreateTerrainColider(const vec3& positionOffset, std::vector<float>& data) = 0;
			virtual uint32 CreateRigidbody(common::Transform&, float mass, bool isStatic) = 0;
		};
	}

} // GameEngine
