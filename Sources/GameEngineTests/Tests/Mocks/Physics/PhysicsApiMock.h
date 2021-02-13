#pragma once
#include <gmock/gmock.h>
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

#include "Types.h"

namespace GameEngine
{
struct PhysicsApiMock : public Physics::IPhysicsApi
{
    MOCK_METHOD0(Simulate, void());
    MOCK_METHOD0(DebugDraw, const GraphicsApi::LineMesh&());
    MOCK_METHOD0(EnableSimulation, void());
    MOCK_METHOD0(DisableSimulation, void());
    MOCK_METHOD1(SetSimulationStep, void(float));
    MOCK_METHOD3(CreateBoxColider, uint32(const vec3&, const vec3&, const vec3&));
    MOCK_METHOD3(CreateSphereColider, uint32(const vec3&, const vec3&, float));
    MOCK_METHOD4(CreateCapsuleColider, uint32(const vec3&, const vec3&, float, float));
    MOCK_METHOD3(CreateTerrainColider, uint32(const vec3&, const vec3&, const HeightMap&));
    MOCK_METHOD5(CreateMeshCollider, uint32(const vec3&, const std::vector<float>&, const IndicesVector&, const vec3&, bool));
    MOCK_METHOD5(CreateRigidbody, uint32(uint32, GameObject&, float, bool, bool&));
    MOCK_METHOD1(RemoveRigidBody, void(uint32));
    MOCK_METHOD1(RemoveShape, void(uint32));
    MOCK_METHOD2(SetVelocityRigidbody, void(uint32, const vec3&));
    MOCK_METHOD2(ApplyImpulse, void(uint32, const vec3&));
    MOCK_METHOD2(IncreaseVelocityRigidbody, void(uint32, const vec3&));
    MOCK_METHOD1(GetVelocity, std::optional<vec3>(uint32));
    MOCK_METHOD2(SetAngularFactor, void(uint32, float));
    MOCK_METHOD2(SetAngularFactor, void(uint32, const vec3&));
    MOCK_METHOD1(GetAngularFactor, std::optional<vec3>(uint32));
    MOCK_METHOD2(SetRotation, void(uint32, const vec3&));
    MOCK_METHOD2(SetRotation, void(uint32, const Quaternion&));
    MOCK_METHOD2(SetPosition, void(uint32, const vec3&));
    MOCK_METHOD2(SetRigidbodyScale, void(Physics::RigidbodyId, const vec3&));
    MOCK_METHOD2(SetShapeScale, void(Physics::ShapeId, const vec3&));
    MOCK_CONST_METHOD1(GetRotation, std::optional<Quaternion>(uint32));
    MOCK_CONST_METHOD1(GetTransfrom, std::optional<common::Transform>(uint32));
    MOCK_CONST_METHOD2(RayTest, std::optional<Physics::RayHit>(const vec3&, const vec3&));
	MOCK_METHOD1(setVisualizatedRigidbody, void(uint32));
	MOCK_METHOD0(enableVisualizationForAllRigidbodys, void());
	MOCK_METHOD0(disableVisualizationForAllRigidbodys, void());
};
}  // namespace GameEngine
