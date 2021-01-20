#pragma once
#include <gtest/gtest.h>
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Time/Time.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Components/ComponentFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Physics/PhysicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/InputManagerMock.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngineTests/Tests/Mocks/Api/FrameBufferMock.h"
#include <Utils/MeasurementHandler.h>
#include <Utils/ThreadSync.h>

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();

    GpuResourceLoader gpuResourceLoader_;
    Utils::MeasurementHandler measurementHandler_;
    Utils::Thread::ThreadSync threadSync_;
    Input::InputManagerMock inputManagerMock_;
    ::testing::NiceMock<GraphicsApi::GraphicsApiMock> graphicsApiMock_;
    GraphicsApi::FrameBufferMock frameBufferMock_;
    ComponentFactoryMock componentFactoryMock_;
    Time time_;
    CameraMock cameraMock_;
    GameObject obj_;
    ResourceManager resourcesManager_;
    ComponentController componentController_;
    Renderer::RenderersManager renderersManager_;
    PhysicsApiMock physicsApiMock_;
    GraphicsApi::WindowApiMock windowApiMock_;
    CameraWrapper cameraWrapper_;
    ComponentContext context_;
    Frustrum frustrum_;
};
