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
    BaseComponent(ComponentsType, ComponentContext&, GameObject&);
    ~BaseComponent() override;

    inline ComponentsType GetType() const override;
    bool IsActive() const override;
    void Activate() override;
    void Deactivate() override;

    void InitFromParams(const std::unordered_map<std::string, std::string>&) override;
    std::unordered_map<ParamName, Param> GetParams() const override;

    GameObject& GetParentGameObject() override;
    const GameObject& getParentGameObject() const override;

protected:
    void RegisterFunction(FunctionType, std::function<void()> func);

protected:
    ComponentsType type_;
    GameObject& thisObject_;
    ComponentContext& componentContext_;

private:
    bool isActive_;
    std::unordered_map<uint32, FunctionType> ids_;
    std::optional<uint32> componentRegistredId_;
};

ComponentsType BaseComponent::GetType() const
{
    return type_;
}
}  // namespace Components
}  // namespace GameEngine
