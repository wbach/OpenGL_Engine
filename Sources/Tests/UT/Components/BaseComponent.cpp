#include "BaseComponent.h"

#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Renderers/RendererFactory.h>
#include <Logger/Log.h>
#include <gmock/gmock.h>

#include <GameEngine/Scene/Scene.hpp>
#include <memory>

using namespace testing;

BaseComponentTestSchould::BaseComponentTestSchould()
    : scene("TestScene")
    , windowApiMock_()
    , threadSync_(measurementHandler_)
    , renderersManager_(graphicsApiMock_, gpuResourceLoader_, measurementHandler_, threadSync_, time_,
                        std::make_unique<RendererFactory>(graphicsApiMock_))
    , guiFactoryEntryParameters_{guiManager_, inputManagerMock_, resourcesManager_, renderersManager_}
    , guiElementFactory_(guiFactoryEntryParameters_)
    , dialogueManager_(guiElementFactory_, guiManager_, gameState_)
    , context_{scene,
               sceneManager,
               graphicsApiMock_,
               gpuResourceLoader_,
               time_,
               inputManagerMock_,
               physicsApiMock_,
               resourcesManager_,
               componentController_,
               renderersManager_,
               guiElementFactory_,
               timerService_,
               dialogueManager_}
    , componentFactory_(context_)
    , obj_{std::make_unique<GameEngine::GameObject>("Test GameObject", componentController_, componentFactory_, gameObjectIdPool)}
{
    EXPECT_CALL(frameBufferMock_, Init()).WillRepeatedly(Return(false));

    EXPECT_CALL(graphicsApiMock_, GetSupportedRenderers())
        .WillOnce(Return(std::vector<GraphicsApi::RendererType>{GraphicsApi::RendererType::SIMPLE}));
    EXPECT_CALL(graphicsApiMock_, CreateFrameBuffer(_)).WillRepeatedly(ReturnRef(frameBufferMock_));
    renderersManager_.Init();
}

BaseComponentTestSchould::~BaseComponentTestSchould()
{
    LOG_DEBUG << "";
}
