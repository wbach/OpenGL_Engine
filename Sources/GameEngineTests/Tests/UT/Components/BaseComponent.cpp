#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
    : graphicsApiMock_()
    , cameraMock_(new CameraMock())
    , obj_("Test GameObject", componentFactoryMock_)
    , resourcesManager_(graphicsApiMock_)
    , renderersManager_(graphicsApiMock_, shaderFactoryMock_)
    , cameraWrapper_(std::unique_ptr<CameraMock>(cameraMock_))
    , context_(time_, inputManagerMock_, cameraWrapper_, physicsApiMock_, resourcesManager_, renderersManager_, componentController_)
{
}

void BaseComponentTestSchould::Init(BaseComponent*)
{
}
