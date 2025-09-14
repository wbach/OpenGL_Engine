#include "BaseComponent.h"

#include <memory>

#include "Engine/EngineContext.h"
#include "Objects/GameObject.h"
#include "Tests/Mocks/Scene/SceneFactoryMock.h"

using namespace testing;

std::unique_ptr<GameEngine::EngineContext> createEngineContext(BaseComponentTestSchould& base)
{
    auto graphicsApiMock = std::make_unique<::testing::NiceMock<GraphicsApi::GraphicsApiMock>>();

    ON_CALL(*graphicsApiMock, GetWindowApi()).WillByDefault(ReturnRef(base.windowApiMock_));
    ON_CALL(base.windowApiMock_, GetDisplayModes()).WillByDefault(ReturnRef(base.displayModes));
    ON_CALL(base.windowApiMock_, GetInputManager()).WillByDefault(ReturnRef(base.inputManagerMock_));

    return std::make_unique<EngineContext>(std::move(graphicsApiMock), std::make_unique<PhysicsApiMock>(),
                                           std::make_unique<SceneFactoryMock>());
}

BaseComponentTestSchould::BaseComponentTestSchould()
    : windowApiMock_()
    , threadSync_(measurementHandler_)
    , engineContext_{createEngineContext(*this)}
    , physicsApiMock_(static_cast<PhysicsApiMock&>(engineContext_->GetPhysicsApi()))
    , graphicsApiMock_(static_cast<::testing::NiceMock<GraphicsApi::GraphicsApiMock>&>(engineContext_->GetGraphicsApi()))
    , scene("test scene")
    , resourcesManager_(graphicsApiMock_, gpuResourceLoader_)
    , renderersManager_(graphicsApiMock_, gpuResourceLoader_, measurementHandler_, threadSync_, time_)
    , cameraWrapper_(cameraMock_)
    , guiFactoryEntryParameters_{guiManager_, inputManagerMock_, resourcesManager_, renderersManager_}
    , guiElementFactory_(guiFactoryEntryParameters_)
    , context_(scene, sceneManager, graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_, cameraWrapper_,
               physicsApiMock_, resourcesManager_, renderersManager_, componentController_, guiElementFactory_, timerService_)
    , componentFactory_(scene, sceneManager, componentController_, graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_,
                        resourcesManager_, renderersManager_, cameraWrapper_, physicsApiMock_, guiElementFactory_, timerService_)
    , obj_{std::make_unique<GameEngine::GameObject>("Test GameObject", componentController_, componentFactory_, gameObjectIdPool)}
{
    EXPECT_CALL(frameBufferMock_, Init()).WillRepeatedly(Return(false));
    EXPECT_CALL(graphicsApiMock_, GetSupportedRenderers())
        .WillOnce(Return(std::vector<GraphicsApi::RendererType>{GraphicsApi::RendererType::SIMPLE}));
    EXPECT_CALL(graphicsApiMock_, CreateFrameBuffer(_)).WillRepeatedly(ReturnRef(frameBufferMock_));
    renderersManager_.Init();
    scene.InitResources(*engineContext_);
}

BaseComponentTestSchould::~BaseComponentTestSchould()
{
}
