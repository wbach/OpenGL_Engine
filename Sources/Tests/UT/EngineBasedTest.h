#pragma once
#include <Logger/Log.h>
#include <Utils/MeasurementHandler.h>
#include <Utils/ThreadSync.h>
#include <Utils/Time/TimerService.h>
#include <gtest/gtest.h>

#include <memory>

#include "Engine/EngineContext.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "IdPool.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Api/InputManagerMock.h"
#include "Tests/Mocks/Api/WindowApiMock.hpp"
#include "Tests/Mocks/MockThreadSync.h"
#include "Tests/Mocks/Physics/PhysicsApiMock.h"
#include "Tests/Mocks/Renderers/RendererFactoryMock.h"
#include "Tests/Mocks/Resources/ModelLoaderFactoryMock.h"
#include "Tests/Mocks/Resources/TextureLoaderMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class EngineBasedTest : public ::testing::Test
{
public:
    EngineBasedTest();
    virtual ~EngineBasedTest();

    void SetUp() override;
    void TearDown() override;

    using GraphicsApiMock = GraphicsApi::GraphicsApiMock;

    Utils::IdPool IdPool;
    Input::InputManagerMock inputManagerMock;
    std::unique_ptr<EngineContext> engineContext;
    std::vector<GraphicsApi::DisplayMode> displayModes;

    Utils::Thread::MockThreadSync* threadSync;
    GraphicsApiMock* graphicsApi;
    GraphicsApi::WindowApiMock windowApiMock;
    RendererFactoryMock* rendererFactory;
    PhysicsApiMock* physicsApi;
    LoaderMock* modelLoaderMock;
    TextureLoaderMock* textureLoader;
    Scene* scene;
};
