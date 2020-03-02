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
