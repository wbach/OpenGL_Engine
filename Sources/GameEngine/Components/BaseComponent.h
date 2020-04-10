#pragma once
#include <atomic>
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
    virtual ~BaseComponent() override;
    inline ComponentsType GetType() const override;
    virtual void InitFromParams(std::unordered_map<std::string, std::string>) override;
    virtual std::unordered_map<ParamName, Param> GetParams() const override;
    bool IsActive() const override;
    void Activate() override;
    void Deactivate() override;

protected:
    inline void RegisterFunction(FunctionType, std::function<void()> func);

protected:
    ComponentsType type_;
    GameObject& thisObject_;
    ComponentContext componentContext_;

private:
    bool isActive_;
    std::unordered_map<uint32, FunctionType> ids_;
    uint32 componentRegistredId_;
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
