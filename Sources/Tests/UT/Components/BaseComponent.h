#pragma once
#include <Logger/Log.h>
#include <Utils/MeasurementHandler.h>
#include <Utils/ThreadSync.h>
#include <Utils/Time/TimerService.h>
#include <gtest/gtest.h>

#include <memory>

#include "Camera/CameraManager.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentFactory.h"
#include "GameEngine/Narrative/Dialogs/DialogueManager.h"
#include "GameEngine/Narrative/GameState.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Time/Time.h"
#include "GraphicsApi/IFrameBuffer.h"
#include "Scene/Navigation/NavigationManager.h"
#include "Tests/Mocks/Api/FrameBufferMock.h"
#include "Tests/Mocks/Api/GraphicsApiMock.h"
#include "Tests/Mocks/Api/InputManagerMock.h"
#include "Tests/Mocks/Api/WindowApiMock.hpp"
#include "Tests/Mocks/Audio/AudioManagerMock.h"
#include "Tests/Mocks/Camera/CameraMock.h"
#include "Tests/Mocks/Gui/GuiElementFactoryMock.h"
#include "Tests/Mocks/Physics/PhysicsApiMock.h"
#include "Tests/Mocks/Resources/GpuResourceLoaderMock.h"
#include "Tests/Mocks/Resources/ModelLoaderFactoryMock.h"
#include "Tests/Mocks/Resources/ResourcesManagerMock.h"
#include "Tests/Mocks/Scene/SceneManagerMock.h"
#include "Tests/Mocks/Scene/TweenManagerMock.h"
#include "Tests/Mocks/Utils/TimerServiceMock.h"

using namespace GameEngine;
using namespace GameEngine::Components;

class BaseComponentTestSchould : public ::testing::Test
{
public:
    BaseComponentTestSchould();
    virtual ~BaseComponentTestSchould();

    Scene scene;
    PhysicsApiMock physicsApiMock_;
    NavigationManager navigationManager;
    TweenManagerMock tweenManager;
    GameState gameState_;
    CameraManager cameraManager_;
    AudioManagerMock audioManager_;
    std::vector<GraphicsApi::DisplayMode> displayModes;
    Utils::IdPool gameObjectIdPool;
    ::testing::NiceMock<GpuResourceLoaderMock> gpuResourceLoader_;
    Utils::MeasurementHandler measurementHandler_;
    Input::InputManagerMock inputManagerMock_;
    GraphicsApi::FrameBufferMock frameBufferMock_;
    Time time_;
    ComponentController componentController_;
    GraphicsApi::WindowApiMock windowApiMock_;

    Frustrum frustrum_;

    Utils::Thread::ThreadSync threadSync_;
    Utils::Time::TimerServiceMock timerService_;
    ResourceManagerMock resourcesManager_;
    ::testing::NiceMock<GraphicsApi::GraphicsApiMock> graphicsApiMock_;
    SceneManagerMock sceneManager;
    Renderer::RenderersManager renderersManager_;
    CameraMock cameraMock_;
    GuiElementFactoryMock guiElementFactory_;
    DialogueManager dialogueManager_;
    ComponentContext context_;
    ComponentFactory componentFactory_;
    std::unique_ptr<GameObject> obj_;

    GuiManager guiManager_;
};
