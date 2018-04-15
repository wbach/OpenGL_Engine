#include "BaseComponent.h"

BaseComponentTestSchould::BaseComponentTestSchould()
	: graphicsApiMock_(std::make_shared<GraphicsApiMock>())
	, resourcesManager_(graphicsApiMock_)
	, renderersManager_(graphicsApiMock_)
{
}

void BaseComponentTestSchould::Init(AbstractComponent * component)
{
	component->SetTime(&time_);
	component->SetObjectPtr(&obj_);
	component->SetResourceManager(&resourcesManager_);
	component->SetRendererManager(&renderersManager_);
	component->SetComponentController(&componentController_);
	component->SetCamera(&camera_);
}
