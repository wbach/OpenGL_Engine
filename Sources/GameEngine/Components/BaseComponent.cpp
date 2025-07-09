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
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.UnRegisterFunction(thisObject_.GetId(), type, id);
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

std::optional<IdType> BaseComponent::getRegisteredFunctionId(FunctionType functionType) const
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        if (type == functionType)
            return id;
    }
    return std::nullopt;
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
    registeredFunctionsIds_.insert(
                {componentContext_.componentController_.RegisterFunction(thisObject_.GetId(), type, func), type});
}

void BaseComponent::changeActivateStateRegisteredFunctions()
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.setActivateStateOfComponentFunction(thisObject_.GetId(), type, id,
                                                                                   isActive_);
    }
}

}  // namespace Components
}  // namespace GameEngine
