#include "BaseComponent.h"

#include <Logger/Log.h>

#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
BaseComponent::BaseComponent(const ComponentType& type, ComponentContext& componentContext, GameObject& gameObject)
    : type_(type)
    , thisObject_(gameObject)
    , componentContext_(componentContext)
    , isActive_(true)
    , componentRegistredId_(0)
{
    componentRegistredId_ = componentContext_.componentController_.RegisterComponent(type_.id, this);
    LOG_DEBUG << type_;
}

BaseComponent::~BaseComponent()
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.UnRegisterFunction(thisObject_.GetId(), type, id);
    }

    if (componentRegistredId_)
    {
        componentContext_.componentController_.UnRegisterComponent(type_.id, *componentRegistredId_);
    }
    else
    {
        LOG_ERROR << "componentRegistredId not set!";
    }
}
ComponentTypeID BaseComponent::GetTypeId() const
{
    return type_.id;
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
void BaseComponent::SetActive(bool is)
{
    isActive_ = is;
    changeActivateStateRegisteredFunctions();
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

std::vector<FieldInfo> BaseComponent::GetFields()
{
    return {};
}

const std::string& BaseComponent::GetTypeName() const
{
    return type_.name;
}
bool BaseComponent::IsActive() const
{
    return isActive_;
}
void BaseComponent::RegisterFunction(FunctionType type, std::function<void()> func, const ComponentController::Dependencies& dependencies)
{
    registeredFunctionsIds_.insert(
        {componentContext_.componentController_.RegisterFunction(thisObject_.GetId(), GetTypeId(), type, func, dependencies), type});
}

void BaseComponent::changeActivateStateRegisteredFunctions()
{
    for (const auto& [id, type] : registeredFunctionsIds_)
    {
        componentContext_.componentController_.setActivateStateOfComponentFunction(thisObject_.GetId(), type, id, isActive_);
    }
}

}  // namespace Components
}  // namespace GameEngine
