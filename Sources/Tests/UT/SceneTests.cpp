#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "Logger/Log.h"
#include "Objects/GameObject.h"
#include "Scene/Scene.hpp"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Scene/SceneFactoryMock.h"
#include "Tests/UT/Components/BaseComponent.h"
#include "gmock/gmock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

using namespace testing;
using namespace GraphicsApi;

using namespace testing;

struct SceneTestSchould : public BaseComponentTestSchould
{
    void SetUp() override
    {
    }
};

TEST_F(SceneTestSchould, AddRemoveGameObjectStability)
{
    for (int i = 0; i < 5000; ++i)
    {
        LOG_DEBUG << "Iteration start: i=" << i;
        auto go  = scene.CreateGameObject("TestGameObjectName_" + std::to_string(i));
        auto ptr = go.get();
        scene.AddGameObject(std::move(go));
        auto gameObject = scene.GetGameObject(ptr->GetId());
        EXPECT_NE(gameObject, nullptr);
        scene.RemoveGameObject(*ptr);
        LOG_DEBUG << "Iteration end: i=" << i;
    }

    SUCCEED();
}