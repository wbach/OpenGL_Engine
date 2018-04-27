#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
	: graphicsApiMock_(std::make_shared<GraphicsApiMock>())
	, camera_(new CameraMock())
	, resourcesManager_(graphicsApiMock_)
	, renderersManager_(graphicsApiMock_)
	, cameraPtr_(camera_)
{
}

void BaseComponentTestSchould::Init(AbstractComponent * component)
{
	component->SetTime(&time_);
	component->SetObjectPtr(&obj_);
	component->SetResourceManager(&resourcesManager_);
	component->SetRendererManager(&renderersManager_);
	component->SetComponentController(&componentController_);
	component->SetCamera(&cameraPtr_);
}
