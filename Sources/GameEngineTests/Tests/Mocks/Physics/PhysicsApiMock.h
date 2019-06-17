#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Physics/IPhysicsApi.h"
#include "Types.h"

namespace GameEngine
{
struct PhysicsApiMock : public Physics::IPhysicsApi
{
    MOCK_METHOD0(Simulate, void());
    MOCK_METHOD2(DebugDraw, void(const mat4&, const mat4&));
    MOCK_METHOD0(EnableSimulation, void());
    MOCK_METHOD0(DisableSimulation, void());
    MOCK_METHOD1(SetSimulationStep, void(float));
    MOCK_METHOD2(CreateBoxColider, uint32(const vec3&, const vec3&));
    MOCK_METHOD2(CreateSphereColider, uint32(const vec3&, float));
    MOCK_METHOD4(CreateTerrainColider, uint32(const vec3&, const vec2ui&, const std::vector<float>&, const vec3&));
    MOCK_METHOD4(CreateMeshCollider, uint32(const vec3&, const std::vector<float>&, const IndicesVector&, float));
    MOCK_METHOD4(CreateRigidbody, uint32(uint32, common::Transform&, float, bool));
    MOCK_METHOD2(SetVelocityRigidbody, void(uint32, const vec3&));
    MOCK_METHOD1(RemoveRigidBody, void(uint32));
};

}  // namespace GameEngine
