#pragma once
#include <gmock/gmock.h>

#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "Types.h"

namespace GameEngine
{
struct PhysicsApiMock : public Physics::IPhysicsApi
{
    MOCK_METHOD1(Simulate, void(float));
    MOCK_METHOD0(DebugDraw, const GraphicsApi::LineMesh&());
    MOCK_METHOD0(EnableSimulation, void());
    MOCK_METHOD0(DisableSimulation, void());
    MOCK_METHOD3(CreateBoxColider, Physics::ShapeId(const vec3&, const vec3&, const vec3&));
    MOCK_METHOD3(CreateSphereColider, Physics::ShapeId(const vec3&, const vec3&, float));
    MOCK_METHOD4(CreateCapsuleColider, Physics::ShapeId(const vec3&, const vec3&, float, float));
    MOCK_METHOD3(CreateTerrainColider, Physics::ShapeId(const vec3&, const vec3&, const HeightMap&));
    MOCK_METHOD5(CreateMeshCollider,
                 Physics::ShapeId(const vec3&, const std::vector<float>&, const IndicesVector&, const vec3&, bool));
    MOCK_METHOD5(CreateRigidbody, Physics::RigidbodyId(const Physics::ShapeId&, GameObject&, float, bool, bool&));
    MOCK_METHOD1(RemoveRigidBody, void(const Physics::RigidbodyId&));
    MOCK_METHOD1(RemoveShape, void(const Physics::ShapeId&));
    MOCK_METHOD2(SetVelocityRigidbody, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD2(ApplyImpulse, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD2(IncreaseVelocityRigidbody, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD1(GetVelocity, std::optional<vec3>(const Physics::RigidbodyId&));
    MOCK_METHOD2(SetAngularFactor, void(const Physics::RigidbodyId&, float));
    MOCK_METHOD2(SetAngularFactor, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD1(GetAngularFactor, std::optional<vec3>(const Physics::RigidbodyId&));
    MOCK_METHOD2(SetRotation, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD2(SetRotation, void(const Physics::RigidbodyId&, const Quaternion&));
    MOCK_METHOD2(SetPosition, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD2(SetRigidbodyScale, void(const Physics::RigidbodyId&, const vec3&));
    MOCK_METHOD2(SetShapeScale, void(const Physics::ShapeId&, const vec3&));
    MOCK_CONST_METHOD1(GetRotation, std::optional<Quaternion>(const Physics::RigidbodyId&));
    MOCK_CONST_METHOD1(GetTransfrom, std::optional<common::Transform>(const Physics::RigidbodyId&));
    MOCK_CONST_METHOD2(RayTest, std::optional<Physics::RayHit>(const vec3&, const vec3&));
    MOCK_METHOD1(setVisualizatedRigidbody, void(const Physics::RigidbodyId&));
    MOCK_METHOD0(enableVisualizationForAllRigidbodys, void());
    MOCK_METHOD0(disableVisualizationForAllRigidbodys, void());
    MOCK_METHOD2(setCollisionCallback,
                 Physics::CollisionSubId(const Physics::RigidbodyId&,
                                         std::function<void(const Physics::CollisionContactInfo&)>));
    MOCK_METHOD1(celarCollisionCallback, void(const Physics::CollisionSubId&));
};
}  // namespace GameEngine
