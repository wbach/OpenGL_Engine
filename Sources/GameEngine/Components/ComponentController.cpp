#include "ComponentController.h"

#include <algorithm>
#include <magic_enum/magic_enum.hpp>
#include <mutex>
#include <queue>
#include <stdexcept>

#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
ComponentController::ComponentController()
    : componentId(0)
    , isStarted(false)
{
}
ComponentController::~ComponentController()
{
    for (auto& [gameObjectId, functionMap] : functions_)
    {
        if (functionMap.size() > 0)
        {
            LOG_WARN << "Some funstion left. GameObjectId : " << gameObjectId;
            for (const auto& [type, _] : functionMap)
            {
                LOG_WARN << " GameObjectId : " << gameObjectId << ". Function type " << magic_enum::enum_name(type);
            }
        }
    }

    for (auto& [type, componentMap] : registredComponents_)
    {
        if (componentMap.size() > 0)
        {
            LOG_WARN << "Some components left. Type : " << type;
        }
    }
}

ComponentController::FunctionId ComponentController::RegisterFunction(GameObjectId gameObjectId, const ComponentType& owner,
                                                                      FunctionType type, std::function<void()> func,
                                                                      const Dependencies& dependencies)
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    auto id = functionIdsPool_.getId();
    functions_[gameObjectId][type].push_back(ComponentFunction{
        .function = func, .meta = FunctionMeta{.id = id, .isActive = true, .ownerType = owner, .dependencies = dependencies}});
    return id;
}

ComponentController::ComponentId ComponentController::RegisterComponent(ComponentTypeID type, IComponent* component)
{
    std::lock_guard<std::mutex> lk(componentsMutex_);
    auto currentComponentId = componentId++;

    registredComponents_[type].insert({currentComponentId, component});
    return currentComponentId;
}

void ComponentController::UnRegisterComponent(ComponentTypeID type, ComponentId id)
{
    std::lock_guard<std::mutex> lk(componentsMutex_);
    auto iter = registredComponents_.find(type);
    if (iter != registredComponents_.end())
    {
        iter->second.erase(id);
        if (iter->second.empty())
        {
            registredComponents_.erase(iter);
        }
    }
    else
    {
        LOG_WARN << "ComponentsType not found.";
    }
}
void ComponentController::UnRegisterFunction(ComponentController::GameObjectId gameObjectId, FunctionType type, IdType id)
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter = std::find_if(typeIter->second.begin(), typeIter->second.end(),
                                             [id](const auto& componentFunction) { return id == componentFunction.meta.id; });
            if (functionIter != typeIter->second.end())
            {
                typeIter->second.erase(functionIter);
            }
            else
            {
                LOG_WARN << "Function not found. Function type = " << magic_enum::enum_name(type) << " funcId = " << id;
            }

            if (typeIter->second.empty())
            {
                iter->second.erase(typeIter);
            }

            functionIdsPool_.releaseId(id);
        }
        else
        {
            LOG_WARN << "Type not found. Function type = " << magic_enum::enum_name(type) << " funcId = " << id;
        }

        if (iter->second.empty())
        {
            functions_.erase(iter);
        }
    }
    else
    {
        LOG_WARN << "Unknow gameObject try unregistger function. Function type = " << magic_enum::enum_name(type)
                 << " funcId = " << id;
    }
}

void ComponentController::setActivateStateOfComponentFunction(ComponentController::GameObjectId gameObjectId, FunctionType type,
                                                              FunctionId id, bool activeStatus)
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter = std::find_if(typeIter->second.begin(), typeIter->second.end(),
                                             [id](const auto& componentFunction) { return id == componentFunction.meta.id; });
            if (functionIter != typeIter->second.end())
            {
                functionIter->meta.isActive = activeStatus;
            }
            else
            {
                LOG_ERROR << "Function id=" << id << "of type{" << magic_enum::enum_name(type)
                          << "} not found for gameObjectId=" << gameObjectId;
            }
        }
        else
        {
            LOG_ERROR << "Function type{" << magic_enum::enum_name(type) << "} not found for gameObjectId=" << gameObjectId;
        }
    }
    else
    {
        LOG_ERROR << "GameObject not found, gameObjectId=" << gameObjectId;
    }
}

void ComponentController::callComponentFunction(ComponentController::GameObjectId gameObjectId, FunctionType type,
                                                ComponentController::FunctionId functionId)
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter =
                std::find_if(typeIter->second.begin(), typeIter->second.end(),
                             [functionId](const auto& componentFunction) { return functionId == componentFunction.meta.id; });

            if (functionIter != typeIter->second.end() and functionIter->function)
            {
                functionIter->function();
            }
            else
            {
                LOG_ERROR << "Can not call function type{" << magic_enum::enum_name(type)
                          << "} for gameObjectId=" << gameObjectId;
            }
        }
        else
        {
            LOG_ERROR << "Function type{" << magic_enum::enum_name(type) << "} not found for gameObjectId=" << gameObjectId;
        }
    }
    else
    {
        LOG_ERROR << "GameObject not found, gameObjectId=" << gameObjectId;
    }
}

void ComponentController::UnRegisterAll()
{
    std::scoped_lock lk(functionsMutex_, componentsMutex_);
    functions_.clear();
    registredComponents_.clear();
}
void ComponentController::OnObjectCreated(IdType gameObjectId)
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    SortAllFunctionsForGameObject(gameObjectId);

    CallGameObjectFunctions(FunctionType::Awake, gameObjectId);
    CallGameObjectFunctions(FunctionType::LateAwake, gameObjectId);

    if (isStarted)
    {
        CallGameObjectFunctions(FunctionType::OnStart, gameObjectId);
        CallGameObjectFunctions(FunctionType::PostStart, gameObjectId);
    }
}
void ComponentController::OnStart()
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    CallFunctions(FunctionType::OnStart);
    CallFunctions(FunctionType::PostStart);
    isStarted = true;
}
void ComponentController::Update()
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    CallFunctions(FunctionType::Update);
}
void ComponentController::PostUpdate()
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    CallFunctions(FunctionType::PostUpdate);
}
void ComponentController::AlwaysUpdate()
{
    std::lock_guard<std::mutex> lk(functionsMutex_);
    CallFunctions(FunctionType::AlwaysUpdate);
}
void ComponentController::CallFunctions(FunctionType type)
{
    for (auto& [_, functionBucket] : functions_)
    {
        auto iter = functionBucket.find(type);

        if (iter != functionBucket.end())
        {
            for (auto& componentFunction : iter->second)
            {
                if (componentFunction.meta.isActive)
                    componentFunction.function();
            }
        }
    }
}

void ComponentController::CallGameObjectFunctions(FunctionType funcType, IdType gameObjectId)
{
    auto gameObjectFunctionsIter = functions_.find(gameObjectId);

    if (gameObjectFunctionsIter != functions_.end())
    {
        auto iter = gameObjectFunctionsIter->second.find(funcType);
        if (iter != gameObjectFunctionsIter->second.end())
        {
            for (auto& componentFunction : iter->second)
            {
                componentFunction.function();
            }
        }
    }
}

// ------------------------------------------------------------
// Sortowanie funkcji po zaleznosciach
// ------------------------------------------------------------
void ComponentController::SortAllFunctionsForGameObject(IdType gameObjectId)
{
    auto objectIter = functions_.find(gameObjectId);
    if (objectIter == functions_.end())
        return;  // brak funkcji dla tego obiektu

    FunctionBucket& functionBucket = objectIter->second;

    // Przechodzimy po wszystkich typach funkcji
    for (auto& [functionType, componentFunctionVec] : functionBucket)
    {
        // Wywolujemy prywatna metode SortFunctions, ktora zwraca wektor wskaznikow posortowanych topologicznie
        std::vector<const ComponentFunction*> sortedFunctionPtrs = SortFunctions(componentFunctionVec);

        // Nadpisujemy oryginalny wektor w bucketcie
        // Zmieniamy typ z std::vector<ComponentFunction> na std::vector<ComponentFunction> zgodnie z potrzeba
        std::vector<ComponentFunction> sortedFunctions;
        sortedFunctions.reserve(sortedFunctionPtrs.size());
        for (const auto* funcPtr : sortedFunctionPtrs)
        {
            sortedFunctions.push_back(*funcPtr);
        }

        componentFunctionVec = std::move(sortedFunctions);
    }
}

std::vector<const ComponentController::ComponentFunction*> ComponentController::SortFunctions(
    const std::vector<ComponentFunction>& registeredFunctions)
{
    std::unordered_map<const ComponentFunction*, int> remainingDependenciesCount;
    std::unordered_map<ComponentTypeID, std::vector<const ComponentFunction*>> dependentsByComponentType;

    std::vector<const ComponentFunction*> globalFunctions;  // funkcje NULL_COMPONENT_ID

    // 1. Budujemy indegree i mape zaleznosci
    for (auto& func : registeredFunctions)
    {
        if (func.meta.ownerType.id == NULL_COMPONENT_TYPE.id)
        {
            globalFunctions.push_back(&func);
        }
        else
        {
            remainingDependenciesCount[&func] = static_cast<int>(func.meta.dependencies.size());
            for (auto dep : func.meta.dependencies)
            {
                dependentsByComponentType[dep.id].push_back(&func);
            }
        }
    }

    std::queue<const ComponentFunction*> readyQueue;
    for (auto& [funcPtr, count] : remainingDependenciesCount)
    {
        if (count == 0)
            readyQueue.push(funcPtr);
    }

    std::vector<const ComponentFunction*> sortedFunctions;
    sortedFunctions.reserve(registeredFunctions.size());

    // 2. Topologiczne sortowanie zwyklych komponentow
    while (!readyQueue.empty())
    {
        const ComponentFunction* current = readyQueue.front();
        readyQueue.pop();
        sortedFunctions.push_back(current);

        for (auto dependent : dependentsByComponentType[current->meta.ownerType.id])
        {
            if (--remainingDependenciesCount[dependent] == 0)
            {
                readyQueue.push(dependent);
            }
        }
    }

    if (sortedFunctions.size() + globalFunctions.size() != registeredFunctions.size())
    {
        std::vector<const ComponentFunction*> unresolved;
        for (auto& [funcPtr, count] : remainingDependenciesCount)
        {
            if (count > 0)
                unresolved.push_back(funcPtr);
        }

        std::unordered_set<ComponentType> existingOwners;
        for (auto& func : registeredFunctions)
        {
            if (func.meta.ownerType.id != NULL_COMPONENT_TYPE.id)
                existingOwners.insert(func.meta.ownerType);
        }

        std::ostringstream missingMsg;
        std::ostringstream cycleMsg;
        bool hasMissingDependency = false;
        bool hasCycle             = false;

        for (auto* func : unresolved)
        {
            for (auto dep : func->meta.dependencies)
            {
                if (existingOwners.find(dep) == existingOwners.end())
                {
                    hasMissingDependency = true;
                    missingMsg << "Function " << func->meta.id << " in " << func->meta.ownerType
                               << " depends on missing " << dep << "\n";
                }
                else
                {
                    hasCycle = true;
                    cycleMsg << "Function " << func->meta.id << " in " << func->meta.ownerType
                             << " participates in cyclic dependency involving " << dep << "\n";
                }
            }
        }

        if (hasMissingDependency)
            LOG_WARN << "Missing dependencies detected:\n" << missingMsg.str();

        if (hasCycle)
            throw std::runtime_error("Cyclic dependencies detected:\n" + cycleMsg.str());
    }
    // 3. Dodajemy na koncu wszystkie funkcje globalne
    sortedFunctions.insert(sortedFunctions.end(), globalFunctions.begin(), globalFunctions.end());

    return sortedFunctions;
}

}  // namespace Components
}  // namespace GameEngine
