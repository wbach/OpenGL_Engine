#pragma once
#include <memory>
#include "ComponentContext.h"
#include "ComponentController.h"
#include "GameEngine/Time/Time.h"
#include "IComponent.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
class BaseComponent : public IComponent
{
public:
    BaseComponent(ComponentsType type, const ComponentContext& ComponentContext, GameObject& gameObject);
    virtual ~BaseComponent();
    inline ComponentsType GetType() const;

protected:
    inline void RegisterFunction(FunctionType, std::function<void()> func);

protected:
    ComponentsType type_;
    GameObject& thisObject_;
    ComponentContext componentContext_;

private:
    std::unordered_map<uint32, FunctionType> ids_;
};

void BaseComponent::RegisterFunction(FunctionType type, std::function<void()> func)
{
    ids_.insert({componentContext_.componentController_.RegisterFunction(type, func), type});
}
ComponentsType BaseComponent::GetType() const
{
    return type_;
}
}  // namespace Components
}  // namespace GameEngine
