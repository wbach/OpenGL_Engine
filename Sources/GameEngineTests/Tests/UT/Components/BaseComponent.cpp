#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
    : graphicsApiMock_(std::make_shared<GraphicsApiMock>())
    , cameraMock_(new CameraMock())
    , resourcesManager_(graphicsApiMock_)
    , renderersManager_(graphicsApiMock_, shaderFactoryMock_)
    , context_(time_, camera_, physicsApiMock_, resourcesManager_, renderersManager_, componentController_)
    , obj_(componentFactoryMock_)
    , camera_(cameraMock_)
{

}

void BaseComponentTestSchould::Init(BaseComponent* component)
{

}
