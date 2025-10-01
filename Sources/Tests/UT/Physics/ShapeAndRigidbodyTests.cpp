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
#include "gmock/gmock.h"

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
    Container<T> CreateAndAddGameObjectWithShape(bool processEvents = true)
    {
        auto go         = scene->CreateGameObject();
        auto ptr        = go.get();
        auto& ref       = go->AddComponent<T>();
        auto& rigidBody = go->AddComponent<Rigidbody>();
        scene->AddGameObject(std::move(go));
        if (processEvents)
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

TEST_F(ShapeAndRigidbodyTests, StabilityTestAddRemoveManyObjects)
{
    scene->Start();
    EXPECT_CALL(*physicsApi, CreateBoxColider(_, _, _)).WillRepeatedly(Return(idPool.getId()));
    EXPECT_CALL(*physicsApi, CreateRigidbody(_, _, _, _, _)).WillRepeatedly(Return(idPool.getId()));
    EXPECT_CALL(*physicsApi, RemoveShape(_)).Times(AtLeast(1));
    EXPECT_CALL(*physicsApi, RemoveRigidBody(_)).Times(AtLeast(1));
    EXPECT_CALL(*physicsApi, SetAngularFactor(_, Matcher<const vec3&>(_))).Times(AtLeast(1));

    std::atomic_bool isrunning{true};
    std::thread removalThread(
        [&]()
        {
            while (isrunning)
            {

                scene->FullUpdate(0.1f);
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100.f)));
            }
        });

    for (int i = 0; i < 1000; ++i)
    {
        auto container = CreateAndAddGameObjectWithShape<BoxShape>(false);
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100.f)));
        scene->RemoveGameObject(container.gameObject.GetId());
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(getRandomFloat() * 100.f)));
    }
    isrunning.store(false);
    removalThread.join();

    LOG_DEBUG << "TestDone";
    SUCCEED();
}