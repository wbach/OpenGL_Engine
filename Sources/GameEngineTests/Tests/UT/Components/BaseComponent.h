#pragma once
#include <Utils/MeasurementHandler.h>
#include <Utils/ThreadSync.h>
#include <Utils/Time/TimerService.h>
#include <gtest/gtest.h>

#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Time/Time.h"
#include "GameEngineTests/Tests/Mocks/Api/FrameBufferMock.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/InputManagerMock.h"
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Physics/PhysicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "GameEngineTests/Tests/Mocks/Scene/SceneManagerMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();
    virtual ~BaseComponentTestSchould();

    Utils::IdPool gameObjectIdPool;
    ::testing::NiceMock<GpuResourceLoaderMock> gpuResourceLoader_;
    Utils::MeasurementHandler measurementHandler_;
    Input::InputManagerMock inputManagerMock_;
    GraphicsApi::FrameBufferMock frameBufferMock_;
    Time time_;
    ComponentController componentController_;
    PhysicsApiMock physicsApiMock_;
    GraphicsApi::WindowApiMock windowApiMock_;
    GuiManager guiManager_;
    Frustrum frustrum_;

    Utils::Thread::ThreadSync threadSync_;
    Utils::Time::TimerService timerService_;
    ::testing::NiceMock<GraphicsApi::GraphicsApiMock> graphicsApiMock_;
    Scene scene;
    SceneManagerMock sceneManager;
    ResourceManager resourcesManager_;
    Renderer::RenderersManager renderersManager_;
    CameraMock cameraMock_;
    CameraWrapper cameraWrapper_;
    GuiElementFactory::EntryParameters guiFactoryEntryParameters_;
    GuiElementFactory guiElementFactory_;
    ComponentContext context_;
    ComponentFactory componentFactory_;
    GameObject obj_;
};
