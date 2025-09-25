#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "Logger/Log.h"
#include "Objects/GameObject.h"
#include "Scene/Scene.hpp"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Scene/SceneFactoryMock.h"
#include "Tests/UT/Components/BaseComponent.h"
#include "Tests/UT/EngineBasedTest.h"
#include "gmock/gmock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;
using namespace GraphicsApi;

using namespace testing;

struct SceneTestSchould : public EngineBasedTest
{
    void SetUp() override
    {
        EngineBasedTest::SetUp();
    }
};

TEST_F(SceneTestSchould, AddRemoveGameObjectStability)
{
    scene->Start();
    engineContext->GetSceneManager().StopThread();

    for (int i = 0; i < 5000; ++i)
    {
        LOG_DEBUG << "Iteration start: i=" << i;
        auto go = scene->CreateGameObject("TestGameObjectName_" + std::to_string(i + 1));
        EXPECT_NE(go, nullptr);
        auto id = go->GetId();
        scene->AddGameObject(std::move(go));
        scene->ProcessEvents();
        LOG_DEBUG << "scene->GetGameObject(id) id =" << id;
        auto gameObject = scene->GetGameObject(id);
        EXPECT_NE(gameObject, nullptr);
        scene->RemoveGameObject(*gameObject);
        scene->ProcessEvents();
        LOG_DEBUG << "Iteration end: i=" << i;
    }

    SUCCEED();
}
