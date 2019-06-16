#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
    : graphicsApiMock_()
    , cameraMock_(new CameraMock())
    , obj_("Test GameObject", componentFactoryMock_)
    , resourcesManager_(graphicsApiMock_)
    , renderersManager_(graphicsApiMock_, shaderFactoryMock_)
    , cameraWrapper_(std::move(std::make_unique<CameraMock>()))
    , context_(time_, inputManagerMock_, cameraWrapper_, physicsApiMock_, resourcesManager_, renderersManager_, componentController_)
{
}

void BaseComponentTestSchould::Init(BaseComponent* component)
{
}
