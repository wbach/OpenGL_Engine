#include "BaseComponent.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
BaseComponent::BaseComponent(ComponentsType type, const ComponentContext& componentContext, GameObject& gameObject)
    : type_(type)
    , thisObject_(gameObject)
    , componentContext_(componentContext)
    , isActive_(true)
    , componentRegistredId_(0)
{
   componentRegistredId_ = componentContext_.componentController_.RegisterComponent(type_, this);
}
BaseComponent::~BaseComponent()
{
 //   DEBUG_LOG("destructor " + std::to_string(type_));

    for (auto id : ids_)
        componentContext_.componentController_.UnRegisterFunction(id.second, id.first);

    if (componentRegistredId_)
        componentContext_.componentController_.UnRegisterComponent(type_, componentRegistredId_);
}
void BaseComponent::Activate()
{
    isActive_ = true;
}
void BaseComponent::Deactivate()
{
    isActive_ = false;
}
std::unordered_map<ParamName, Param> BaseComponent::GetParams() const
{
    return {};
}
bool BaseComponent::IsActive() const
{
    return isActive_;
}
void BaseComponent::InitFromParams(std::unordered_map<std::string, std::string>)
{
}
}  // namespace Components
}  // namespace GameEngine
