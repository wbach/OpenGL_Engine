#pragma once
#include <Types.h>
#include <Utils/IdPool.h>

#include <functional>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "ComponentDependencies.h"
#include "FunctionType.h"
#include "GameEngine/Components/ComponentType.h"
#include "IComponent.h"

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
    std::vector<T*> GetAllComponentsOfType() const;
    template <class T>
    std::vector<T*> GetAllActiveComponentsOfType() const;

    FunctionId RegisterFunction(GameObjectId, const ComponentType&, FunctionType, std::function<void()>,
                                const Dependencies& = {});
    void UnRegisterFunction(GameObjectId, FunctionType, FunctionId);
    // TO DO: FIX set active during component function
    void setActivateStateOfComponentFunction(GameObjectId, FunctionType, FunctionId, bool);

    void callComponentFunction(GameObjectId, FunctionType, FunctionId);
    void CallFunctions(FunctionType);
    void CallGameObjectFunctions(FunctionType, IdType);

    ComponentId RegisterComponent(ComponentTypeID, IComponent*);
    void UnRegisterComponent(ComponentTypeID, ComponentId);
    void UnRegisterAll();

    const ComponentFunctions& getComponentFunctions() const;
    const ComponentsContainer& getComponentsContainer() const;

    void OnObjectCreated(IdType);
    void OnStart();
    void Update();
    void PostUpdate();
    void AlwaysUpdate();

private:
    void MergeTmpFunctionListToMain();
    void CallGameObjectFunctionsImpl(FunctionType funcType, IdType gameObjectId);
    void SortAllFunctionsForGameObject(IdType gameObjectId);
    std::vector<const ComponentFunction*> SortFunctions(const std::vector<ComponentFunction>& funcs);

private:
    ComponentFunctions functions_;
    ComponentFunctions tmpfunctions_;
    ComponentsContainer registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;
    std::set<GameObjectId> startedGameObjects_;
    std::mutex functionsMutex_;
    std::mutex tmpfunctionsMutex_;
    std::mutex componentsMutex_;

private:
    const RegistredComponentsMap DEFAULT_COMPONETNS_MAP;
};

template <class T>
std::vector<T*> ComponentController::GetAllActiveComponentsOfType() const
{
    const ComponentTypeID componentTypeId = GetComponentType<T>().id;

    auto registeredComponentsIterator = registredComponents_.find(componentTypeId);
    if (registeredComponentsIterator == registredComponents_.end())
    {
        return {};
    }

    const RegistredComponentsMap& componentsMap = registeredComponentsIterator->second;

    std::vector<T*> componentsOfType;
    componentsOfType.reserve(componentsMap.size());

    for (const auto& [componentId, componentPtr] : componentsMap)
    {
        if (componentPtr->IsActive())
            componentsOfType.push_back(static_cast<T*>(componentPtr));
    }

    return componentsOfType;
}

template <class T>
std::vector<T*> ComponentController::GetAllComponentsOfType() const
{
    const ComponentTypeID componentTypeId = GetComponentType<T>().id;

    auto registeredComponentsIterator = registredComponents_.find(componentTypeId);
    if (registeredComponentsIterator == registredComponents_.end())
    {
        return {};
    }

    const RegistredComponentsMap& componentsMap = registeredComponentsIterator->second;

    std::vector<T*> componentsOfType;
    componentsOfType.reserve(componentsMap.size());

    for (const auto& [componentId, componentPtr] : componentsMap)
    {
        componentsOfType.push_back(static_cast<T*>(componentPtr));
    }

    return componentsOfType;
}

}  // namespace Components
}  // namespace GameEngine
