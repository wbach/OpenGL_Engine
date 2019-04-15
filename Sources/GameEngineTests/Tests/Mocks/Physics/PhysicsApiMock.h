#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Types.h"

namespace GameEngine
{
struct PhysicsApiMock : public Physics::IPhysicsApi
{
  MOCK_METHOD0(Simulate, void());
  MOCK_METHOD2(DebugDraw, void(const mat4& viewMatrix, const mat4& projectionMatrix));
  MOCK_METHOD0(EnableSimulation, void());
  MOCK_METHOD0(DisableSimulation, void());
  MOCK_METHOD1(SetSimulationStep, void(float step));
  MOCK_METHOD2(CreateBoxColider, uint32(const vec3& positionOffset, const vec3& size));
  MOCK_METHOD2(CreateSphereColider, uint32(const vec3& positionOffset, float radius));
  MOCK_METHOD4(CreateTerrainColider, uint32(const vec3& positionOffset, const vec2ui& size, const std::vector<float>& data, float hightFactor));
  MOCK_METHOD4(CreateMeshCollider,
      uint32(const vec3& positionOffset, const std::vector<float>& data, const IndicesVector& indicies, float scaleFactor));
  MOCK_METHOD4(CreateRigidbody,
      uint32(uint32 shapeId, common::Transform&, float mass, bool isStatic));
  MOCK_METHOD2(SetVelocityRigidbody,
      void(uint32 rigidBodyId, const vec3& velocity));
  MOCK_METHOD1(RemoveRigidBody,
      void(uint32 id));
};

}  // namespace GameEngine
