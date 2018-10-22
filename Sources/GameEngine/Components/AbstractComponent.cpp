#include "AbstractComponent.h"

namespace GameEngine
{
namespace Components
{
void AbstractComponent::SetComponentController(ComponentController* componentController)
{
    componentController_ = componentController;
}
void AbstractComponent::SetCamera(std::shared_ptr<ICamera>* camera)
{
    camera_ = camera;
}
void AbstractComponent::SetPhysicsApi(Physics::IPhysicsApi* api)
{
    physicsApi_ = api;
}
AbstractComponent::AbstractComponent(ComponentsType type)
    : type_(type)
{
}
AbstractComponent::~AbstractComponent()
{
    if (componentController_ == nullptr)
        return;

    for (auto id : ids_)
        componentController_->UnRegisterFunction(id.second, id.first);
}
void AbstractComponent::SetTime(Time* time)
{
    time_ = time;
}
void AbstractComponent::SetObjectPtr(GameObject* ptr)
{
    thisObject = ptr;
}
void AbstractComponent::SetResourceManager(IResourceManager* manager)
{
    resourceManager_ = manager;
}
void AbstractComponent::SetRendererManager(Renderer::RenderersManager* manager)
{
    renderersManager_ = manager;
}
}  // Components
}  // GameEngine
