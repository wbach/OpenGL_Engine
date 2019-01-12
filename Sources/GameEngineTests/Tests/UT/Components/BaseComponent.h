#pragma once
#include <gtest/gtest.h>
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Time/Time.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"
#include "GameEngineTests/Tests/Mocks/Components/ComponentFactoryMock.h"
#include "GameEngineTests/Tests/Mocks/Physics/PhysicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Shaders/ShaderFactoryMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();
    void Init(BaseComponent* component);

    std::shared_ptr<GraphicsApiMock> graphicsApiMock_;
    ComponentFactoryMock componentFactoryMock_;
    Time time_;
    CameraMock* cameraMock_;
    GameObject obj_;
    ResourceManager resourcesManager_;
    ComponentController componentController_;
    Renderer::RenderersManager renderersManager_;
    ShaderFactoryMock shaderFactoryMock_;
    PhysicsApiMock physicsApiMock_;
    ComponentContext context_;
    std::unique_ptr<ICamera> camera_;
};
