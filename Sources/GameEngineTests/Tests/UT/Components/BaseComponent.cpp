#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
    : threadSync_(measurementHandler_)
    , graphicsApiMock_()
    , cameraMock_(new CameraMock())
    , obj_("Test GameObject", componentFactoryMock_, IdType(0))
    , resourcesManager_(graphicsApiMock_, gpuResourceLoader_)
    , renderersManager_(graphicsApiMock_, measurementHandler_, threadSync_, time_)
    , cameraWrapper_(*cameraMock_)
    , context_(graphicsApiMock_, gpuResourceLoader_, time_, inputManagerMock_, cameraWrapper_, physicsApiMock_, resourcesManager_, renderersManager_,
               componentController_)
{
}

void BaseComponentTestSchould::Init(BaseComponent*)
{
}
