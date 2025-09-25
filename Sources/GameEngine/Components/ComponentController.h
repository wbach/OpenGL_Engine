#pragma once
#include <Utils/IdPool.h>

#include <functional>
#include <set>
#include <unordered_map>
#include <vector>

#include "FunctionType.h"
#include "IComponent.h"
#include "Types.h"

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
    using Dependencies = std::vector<ComponentType>;

    struct FunctionMeta
    {
        FunctionId id{0};
        bool isActive{true};
        ComponentType ownerType;
        Dependencies dependencies;
    };
    struct ComponentFunction
    {
        std::function<void()> function;
        FunctionMeta meta;
    };

    using FunctionBucket      = std::unordered_map<FunctionType, std::vector<ComponentFunction>>;
    using ComponentFunctions  = std::unordered_map<GameObjectId, FunctionBucket>;
    using ComponentsContainer = std::unordered_map<ComponentTypeID, RegistredComponentsMap>;

    ComponentController();
    ~ComponentController();

    template <class T>
    std::vector<T*> GetAllComonentsOfType() const
    {
        auto iter = registredComponents_.find(typeid(T).hash_code());

        if (iter != registredComponents_.end())
        {
            std::vector<T*> result;
            result.reserve(iter->second.size());
            for (const auto& [_, component] : iter->second)
            {
                result.push_back(static_cast<T*>(component));
            }
            return result;
        }
        return {};
    }

    FunctionId RegisterFunction(GameObjectId, const ComponentType&, FunctionType, std::function<void()>, const Dependencies& = {});
    void UnRegisterFunction(GameObjectId, FunctionType, FunctionId);
    void setActivateStateOfComponentFunction(GameObjectId, FunctionType, FunctionId, bool);
    void callComponentFunction(GameObjectId, FunctionType, FunctionId);

    ComponentId RegisterComponent(ComponentTypeID, IComponent*);
    void UnRegisterComponent(ComponentTypeID, ComponentId);
    void UnRegisterAll();

    const ComponentFunctions& getComponentFunctions() const
    {
        return functions_;
    }
    const ComponentsContainer& getComponentsContainer() const
    {
        return registredComponents_;
    }

public:
    void OnObjectCreated(IdType);
    void OnStart();
    void Update();
    void PostUpdate();
    void AlwaysUpdate();
    void CallFunctions(FunctionType);
    void CallGameObjectFunctions(FunctionType, IdType);

private:
    void SortAllFunctionsForGameObject(IdType gameObjectId);
    std::vector<const ComponentFunction*> SortFunctions(const std::vector<ComponentFunction>& funcs);

private:
    ComponentFunctions functions_;
    ComponentsContainer registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;
    std::set<GameObjectId> startedGameObjects_;

private:
    const RegistredComponentsMap DEFAULT_COMPONETNS_MAP;
};
}  // namespace Components
}  // namespace GameEngine
