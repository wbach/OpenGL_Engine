#pragma once
#include <gtest/gtest.h>
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Time/Time.h"
#include "GameEngineTests/Tests/Mocks/Api/GraphicsApiMock.h"
#include "GameEngineTests/Tests/Mocks/Camera/CameraMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();
    void Init(AbstractComponent* component);

    std::shared_ptr<GraphicsApiMock> graphicsApiMock_;
    Time time_;
    CameraMock* camera_;
    GameObject obj_;
    ResourceManager resourcesManager_;
    ComponentController componentController_;
    Renderer::RenderersManager renderersManager_;

private:
    std::shared_ptr<ICamera> cameraPtr_;
};
