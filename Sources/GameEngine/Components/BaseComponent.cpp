#include "BaseComponent.h"

namespace GameEngine
{
namespace Components
{
BaseComponent::BaseComponent(ComponentsType type, const ComponentContext& componentContext, GameObject& gameObject)
    : type_(type)
    , thisObject_(gameObject)
    , componentContext_(componentContext)
    , isActive_(true)
{
}
BaseComponent::~BaseComponent()
{
    for (auto id : ids_)
        componentContext_.componentController_.UnRegisterFunction(id.second, id.first);
}
void BaseComponent::Activate()
{
    isActive_ = true;
}
void BaseComponent::Deactivate()
{
    isActive_ = false;
}
bool BaseComponent::IsActive() const
{
    return isActive_;
}
}  // Components
}  // GameEngine
