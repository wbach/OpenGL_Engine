#include "ComponentController.h"

#include <algorithm>

#include "Logger/Log.h"

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
    DEBUG_LOG("destructor");

    for (auto& [type, functionMap] : functions_)
    {
        if (functionMap.size() > 0)
        {
            WARNING_LOG("Some funstion left. GameObjectId := " + std::to_string(type));
        }
    }

    for (auto& [type, componentMap] : registredComponents_)
    {
        if (componentMap.size() > 0)
        {
            WARNING_LOG("Some components left. Type : " + std::to_string(type));
        }
    }
}

ComponentController::FunctionId ComponentController::RegisterFunction(GameObjectId gameObjectId, FunctionType type,
                                                                      std::function<void()> func)
{
    auto id = functionIdsPool_.getId();
    functions_[gameObjectId][type].push_back({id, true, func});
    return id;
}

ComponentController::ComponentId ComponentController::RegisterComponent(IComponent::Type type, IComponent* component)
{
    auto currentComponentId = componentId++;

    registredComponents_[type].insert({currentComponentId, component});
    return currentComponentId;
}

void ComponentController::UnRegisterComponent(IComponent::Type type, ComponentId id)
{
    auto iter = registredComponents_.find(type);
    if (iter != registredComponents_.end())
    {
        iter->second.erase(id);
    }
    else
    {
        WARNING_LOG("ComponentsType not found.");
    }
}
void ComponentController::UnRegisterFunction(ComponentController::GameObjectId gameObjectId, FunctionType type, uint32 id)
{
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter = std::find_if(typeIter->second.begin(), typeIter->second.end(),
                                             [id](const auto& componentFunction) { return id == componentFunction.id; });
            if (functionIter != typeIter->second.end())
            {
                typeIter->second.erase(functionIter);
            }

            functionIdsPool_.releaseId(id);
        }
    }
    else
    {
        WARNING_LOG("Function not found.");
    }
}

void ComponentController::setActivateStateOfComponentFunction(ComponentController::GameObjectId gameObjectId, FunctionType type,
                                                              FunctionId id, bool activeStatus)
{
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter = std::find_if(typeIter->second.begin(), typeIter->second.end(),
                                             [id](const auto& componentFunction) { return id == componentFunction.id; });
            if (functionIter != typeIter->second.end())
            {
                functionIter->isActive = activeStatus;
            }
            else
            {
                ERROR_LOG("Function id=" + std::to_string(id) + "of type{" + std::to_string(static_cast<int>(type)) +
                          "} not found for gameObjectId=" + std::to_string(gameObjectId));
            }
        }
        else
        {
            ERROR_LOG("Function type{" + std::to_string(static_cast<int>(type)) +
                      "} not found for gameObjectId=" + std::to_string(gameObjectId));
        }
    }
    else
    {
        ERROR_LOG("GameObject not found, gameObjectId=" + std::to_string(gameObjectId));
    }
}

void ComponentController::callComponentFunction(ComponentController::GameObjectId gameObjectId, FunctionType type,
                                                ComponentController::FunctionId functionId)
{
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter =
                std::find_if(typeIter->second.begin(), typeIter->second.end(),
                             [functionId](const auto& componentFunction) { return functionId == componentFunction.id; });

            if (functionIter != typeIter->second.end() and functionIter->function)
            {
                functionIter->function();
            }
            else
            {
                ERROR_LOG("Can not call function type{" + std::to_string(static_cast<int>(type)) +
                          "} for gameObjectId=" + std::to_string(gameObjectId));
            }
        }
        else
        {
            ERROR_LOG("Function type{" + std::to_string(static_cast<int>(type)) +
                      "} not found for gameObjectId=" + std::to_string(gameObjectId));
        }
    }
    else
    {
        ERROR_LOG("GameObject not found, gameObjectId=" + std::to_string(gameObjectId));
    }
}
void ComponentController::UnRegisterAll()
{
    functions_.clear();
    registredComponents_.clear();
}
void ComponentController::OnObjectCreated(IdType gameObjectId)
{
    CallGameObjectFunctions(FunctionType::Awake, gameObjectId);

    if (isStarted)
    {
        CallGameObjectFunctions(FunctionType::OnStart, gameObjectId);
        CallGameObjectFunctions(FunctionType::PostStart, gameObjectId);
    }
}
void ComponentController::OnStart()
{
    CallFunctions(FunctionType::OnStart);
    CallFunctions(FunctionType::PostStart);
    isStarted = true;
}
void ComponentController::Update()
{
    CallFunctions(FunctionType::Update);
}
void ComponentController::PostUpdate()
{
    CallFunctions(FunctionType::PostUpdate);
}
void ComponentController::AlwaysUpdate()
{
    CallFunctions(FunctionType::AlwaysUpdate);
}
void ComponentController::CallFunctions(FunctionType type)
{
    for (auto& pair : functions_)
    {
        auto iter = pair.second.find(type);

        if (iter != pair.second.end())
        {
            for (auto& componentFunction : iter->second)
            {
                if (componentFunction.isActive)
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
}  // namespace Components
}  // namespace GameEngine
