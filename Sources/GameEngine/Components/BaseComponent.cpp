#include "BaseComponent.h"

namespace GameEngine
{
namespace Components
{
void BaseComponent::SetComponentController(ComponentController* componentController)
{
    componentController_ = componentController;
}
void BaseComponent::SetCamera(std::shared_ptr<ICamera>* camera)
{
    camera_ = camera;
}
void BaseComponent::SetPhysicsApi(Physics::IPhysicsApi* api)
{
    physicsApi_ = api;
}
BaseComponent::BaseComponent(ComponentsType type)
    : type_(type)
{
}
BaseComponent::~BaseComponent()
{
    if (componentController_ == nullptr)
        return;

    for (auto id : ids_)
        componentController_->UnRegisterFunction(id.second, id.first);
}
void BaseComponent::SetTime(Time* time)
{
    time_ = time;
}
void BaseComponent::SetObjectPtr(GameObject* ptr)
{
    thisObject = ptr;
}
void BaseComponent::SetResourceManager(IResourceManager* manager)
{
    resourceManager_ = manager;
}
void BaseComponent::SetRendererManager(Renderer::RenderersManager* manager)
{
    renderersManager_ = manager;
}
}  // Components
}  // GameEngine
