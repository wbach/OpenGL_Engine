#pragma once
#include <Utils/IdPool.h>

#include <functional>
#include <set>
#include <unordered_map>

#include "IComponent.h"
#include "Types.h"
#include "FunctionType.h"

namespace GameEngine
{
namespace Components
{
typedef std::unordered_map<uint32, IComponent*> RegistredComponentsMap;

class ComponentController final
{
public:
    using FunctionId   = IdType;
    using ComponentId  = IdType;
    using GameObjectId = IdType;

    struct ComponentFunction
    {
        FunctionId id{0};
        bool isActive{true};
        std::function<void()> function;
    };

    using ComponentFunctions =
        std::unordered_map<GameObjectId, std::unordered_map<FunctionType, std::vector<ComponentFunction>>>;

    ComponentController();
    ~ComponentController();

    template <class T>
    const RegistredComponentsMap& GetAllComonentsOfType() const
    {
        auto iter = registredComponents_.find(typeid(T).hash_code());

        if (iter != registredComponents_.end())
        {
            return iter->second;
        }

        return DEFAULT_COMPONETNS_MAP;
    }

    FunctionId RegisterFunction(GameObjectId, FunctionType, std::function<void()>);
    void UnRegisterFunction(GameObjectId, FunctionType, FunctionId);
    void setActivateStateOfComponentFunction(GameObjectId, FunctionType, FunctionId, bool);
    void callComponentFunction(GameObjectId, FunctionType, FunctionId);

    ComponentId RegisterComponent(IComponent::Type, IComponent*);
    void UnRegisterComponent(IComponent::Type, ComponentId);

    void UnRegisterAll();

public:
    void OnObjectCreated(IdType);
    void OnStart();
    void Update();
    void PostUpdate();
    void AlwaysUpdate();
    void CallFunctions(FunctionType);
    void CallGameObjectFunctions(FunctionType, IdType);

private:
    ComponentFunctions functions_;
    std::unordered_map<IComponent::Type, RegistredComponentsMap> registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;
    std::set<GameObjectId> startedGameObjects_;

private:
    const RegistredComponentsMap DEFAULT_COMPONETNS_MAP;
};
}  // namespace Components
}  // namespace GameEngine
