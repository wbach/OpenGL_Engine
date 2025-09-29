#include <GameEngine/Components/Physics/BoxShape.h>
#include <GameEngine/Components/Physics/MeshShape.h>
#include <GameEngine/Components/Physics/Rigidbody.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Physics/IPhysicsApi.h>
#include <Logger/Log.h>
#include <Types.h>
#include <Utils/IdPool.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>

#include "Tests/UT/EngineBasedTest.h"

using namespace GameEngine;
using namespace GameEngine::Physics;
using namespace testing;

class ShapeAndRigidbodyTests : public EngineBasedTest
{
public:
    void SetUp() override
    {
        EngineBasedTest::SetUp();
    }

    template <class T>
    struct Container
    {
        T& shape;
        Rigidbody& rigidBody;
        GameObject& gameObject;
    };

    template <class T>
    Container<T> CreateAndAddGameObjectWithShape()
    {
        auto go         = scene->CreateGameObject();
        auto ptr        = go.get();
        auto& ref       = go->AddComponent<T>();
        auto& rigidBody = go->AddComponent<Rigidbody>();
        scene->AddGameObject(std::move(go));
        scene->ProcessEvents();
        return Container<T>{.shape = ref, .rigidBody = rigidBody, .gameObject = *ptr};
    }

    Utils::IdPool idPool{1};
};

TEST_F(ShapeAndRigidbodyTests, ReloadBoxShape)
{
    Physics::ShapeId shapeId         = idPool.getId();
    Physics::RigidbodyId rigidbodyId = idPool.getId();
    EXPECT_CALL(*physicsApi, CreateBoxColider(_, _, _)).WillRepeatedly(Return(shapeId));
    EXPECT_CALL(*physicsApi, CreateRigidbody(_, _, _, _, _)).WillRepeatedly(Return(rigidbodyId));

    auto container = CreateAndAddGameObjectWithShape<BoxShape>();

    EXPECT_CALL(*physicsApi, RemoveShape(shapeId));
    EXPECT_CALL(*physicsApi, RemoveRigidBody(rigidbodyId));

    Physics::ShapeId shapeIdAfterReload         = idPool.getId();
    Physics::RigidbodyId rigidbodyIdAfterReload = idPool.getId();
    EXPECT_CALL(*physicsApi, CreateBoxColider(_, _, _)).WillRepeatedly(Return(shapeIdAfterReload));
    EXPECT_CALL(*physicsApi, CreateRigidbody(_, _, _, _, _)).WillRepeatedly(Return(rigidbodyIdAfterReload));

    LOG_DEBUG << "Reload shape";
    container.shape.Reload();

    EXPECT_CALL(*physicsApi, RemoveShape(shapeIdAfterReload));
    EXPECT_CALL(*physicsApi, RemoveRigidBody(rigidbodyIdAfterReload));

    LOG_DEBUG << "End test";
}

TEST_F(ShapeAndRigidbodyTests, RemoveShapeIfRigidbodyStillExist)
{
    Physics::ShapeId shapeId         = idPool.getId();
    Physics::RigidbodyId rigidbodyId = idPool.getId();
    EXPECT_CALL(*physicsApi, CreateBoxColider(_, _, _)).WillRepeatedly(Return(shapeId));
    EXPECT_CALL(*physicsApi, CreateRigidbody(_, _, _, _, _)).WillRepeatedly(Return(rigidbodyId));

    auto container = CreateAndAddGameObjectWithShape<BoxShape>();

    EXPECT_CALL(*physicsApi, RemoveShape(shapeId));
    EXPECT_CALL(*physicsApi, RemoveRigidBody(rigidbodyId));

    container.shape.CleanUp();
    container.gameObject.RemoveComponent<BoxShape>();

    auto rigidBody = container.gameObject.GetComponent<Rigidbody>();

    EXPECT_NE(rigidBody, nullptr);
    EXPECT_EQ(rigidBody->GetId(), std::nullopt);
}

TEST_F(ShapeAndRigidbodyTests, RemoveRigidbodyIfShapeStillExist)
{
    Physics::ShapeId shapeId         = idPool.getId();
    Physics::RigidbodyId rigidbodyId = idPool.getId();
    EXPECT_CALL(*physicsApi, CreateBoxColider(_, _, _)).WillRepeatedly(Return(shapeId));
    EXPECT_CALL(*physicsApi, CreateRigidbody(_, _, _, _, _)).WillRepeatedly(Return(rigidbodyId));

    auto container = CreateAndAddGameObjectWithShape<BoxShape>();

    EXPECT_CALL(*physicsApi, RemoveShape(shapeId));
    EXPECT_CALL(*physicsApi, RemoveRigidBody(rigidbodyId));

    container.rigidBody.CleanUp();
    container.gameObject.RemoveComponent<Rigidbody>();

    auto shape = container.gameObject.GetComponent<BoxShape>();

    EXPECT_NE(shape, nullptr);
    EXPECT_EQ(shape->GetCollisionShapeId(), std::nullopt);
}