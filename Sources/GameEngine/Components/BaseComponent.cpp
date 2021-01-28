#include "BaseComponent.h"
#include <Logger/Log.h>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
BaseComponent::BaseComponent(size_t type, ComponentContext& componentContext, GameObject& gameObject)
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
    DEBUG_LOG("destructor " + std::to_string(type_));

    for (auto id : ids_)
    {
        componentContext_.componentController_.UnRegisterFunction(thisObject_.GetId(), id.second, id.first);
    }

    if (componentRegistredId_)
    {
        componentContext_.componentController_.UnRegisterComponent(type_, *componentRegistredId_);
    }
    else
    {
        ERROR_LOG("componentRegistredId not set!");
    }
}
void BaseComponent::Activate()
{
    isActive_ = true;
    changeActivateStateRegisteredFunctions();
}
void BaseComponent::Deactivate()
{
    isActive_ = false;
    changeActivateStateRegisteredFunctions();
}
std::unordered_map<ParamName, Param> BaseComponent::GetParams() const
{
    return {};
}
GameObject& BaseComponent::GetParentGameObject()
{
    return thisObject_;
}
const GameObject& BaseComponent::getParentGameObject() const
{
    return thisObject_;
}
void BaseComponent::write(TreeNode&) const
{
}
bool BaseComponent::IsActive() const
{
    return isActive_;
}
void BaseComponent::InitFromParams(const std::unordered_map<std::string, std::string>&)
{
}
void BaseComponent::RegisterFunction(FunctionType type, std::function<void()> func)
{
    ids_.insert({componentContext_.componentController_.RegisterFunction(thisObject_.GetId(), type, func), type});
}

void BaseComponent::changeActivateStateRegisteredFunctions()
{
    for (auto id : ids_)
    {
        componentContext_.componentController_.setActivateStateOfComponentFunction(thisObject_.GetId(), id.second,
                                                                                   id.first, isActive_);
    }
}

}  // namespace Components
}  // namespace GameEngine
