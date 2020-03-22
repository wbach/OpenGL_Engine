#pragma once
#include <gtest/gtest.h>
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Time/Time.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/WindowApiMock.hpp"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Components/ComponentFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Physics/PhysicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Api/InputManagerMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();
    void Init(BaseComponent*);

    Input::InputManagerMock inputManagerMock_;
    GraphicsApi::GraphicsApiMock graphicsApiMock_;
    ComponentFactoryMock componentFactoryMock_;
    Time time_;
    CameraMock* cameraMock_;
    GameObject obj_;
    ResourceManager resourcesManager_;
    ComponentController componentController_;
    Renderer::RenderersManager renderersManager_;
    PhysicsApiMock physicsApiMock_;
    GraphicsApi::WindowApiMock windowApiMock_;
    CameraWrapper cameraWrapper_;
    ComponentContext context_;
};
