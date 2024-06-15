#include "BaseComponent.h"

using namespace testing;

BaseComponentTestSchould::BaseComponentTestSchould()
    : threadSync_(measurementHandler_)
    , graphicsApiMock_()
    , scene("test scene")
    , resourcesManager_(graphicsApiMock_, gpuResourceLoader_)
    , renderersManager_(graphicsApiMock_, gpuResourceLoader_, measurementHandler_, threadSync_, time_)
    , cameraWrapper_(cameraMock_)
    , guiFactoryEntryParameters_{guiManager_, inputManagerMock_, resourcesManager_, renderersManager_}
    , guiElementFactory_(guiFactoryEntryParameters_)
    , context_(scene, graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_, cameraWrapper_, physicsApiMock_,
               resourcesManager_, renderersManager_, componentController_, guiElementFactory_, timerService_)
    , componentFactory_(scene, componentController_, graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_,
                        resourcesManager_, renderersManager_, cameraWrapper_, physicsApiMock_, guiElementFactory_, timerService_)
    , obj_("Test GameObject", componentController_, componentFactory_, IdType(0))
{
    EXPECT_CALL(frameBufferMock_, Init()).WillRepeatedly(Return(false));
    EXPECT_CALL(graphicsApiMock_, GetSupportedRenderers())
        .WillOnce(Return(std::vector<GraphicsApi::RendererType>{GraphicsApi::RendererType::SIMPLE}));
    EXPECT_CALL(graphicsApiMock_, CreateFrameBuffer(_)).WillRepeatedly(ReturnRef(frameBufferMock_));
    renderersManager_.Init();
}

BaseComponentTestSchould::~BaseComponentTestSchould()
{
}
