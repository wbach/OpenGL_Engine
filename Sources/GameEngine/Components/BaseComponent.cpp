#include "BaseComponent.h"

#include <Logger/Log.h>

#include "GameEngine/Objects/GameObject.h"

namespace
{
constexpr uint64_t fnv1a(const char* str)
{
    uint64_t hash = 1469598103934665603ULL;
    while (*str)
    {
        hash ^= static_cast<uint64_t>(*str++);
        hash *= 1099511628211ULL;
    }
    return hash;
}
}  // namespace

namespace GameEngine
{
namespace Components
{
IComponent::Type BaseComponent::GetType(const std::string & type)
{
    return fnv1a(type.c_str());
}

BaseComponent::BaseComponent(const std::string& type, ComponentContext& componentContext, GameObject& gameObject)
    : type_(BaseComponent::GetType(type))
    , name_{type}
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
size_t BaseComponent::GetType() const
{
    return type_;
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

const std::string& BaseComponent::GetTypeString() const
{
    return name_;
}
bool BaseComponent::IsActive() const
{
    return isActive_;
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
        componentContext_.componentController_.setActivateStateOfComponentFunction(thisObject_.GetId(), type, id, isActive_);
    }
}

}  // namespace Components
}  // namespace GameEngine
