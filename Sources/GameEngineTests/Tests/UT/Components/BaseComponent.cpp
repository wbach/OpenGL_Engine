#include "BaseComponent.h"

using namespace testing;

BaseComponentTestSchould::BaseComponentTestSchould()
    : threadSync_(measurementHandler_)
    , graphicsApiMock_()
    , obj_("Test GameObject", componentController_, componentFactoryMock_, IdType(0))
    , resourcesManager_(graphicsApiMock_, gpuResourceLoader_)
    , renderersManager_(graphicsApiMock_, gpuResourceLoader_, measurementHandler_, threadSync_, time_)
    , cameraWrapper_(cameraMock_)
    , context_(graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_, cameraWrapper_, physicsApiMock_, resourcesManager_, renderersManager_,
               componentController_)
{
    EXPECT_CALL(graphicsApiMock_, GetSupportedRenderers()).WillOnce(Return(std::vector<GraphicsApi::RendererType>{ GraphicsApi::RendererType::SIMPLE }));
    renderersManager_.Init();
}
