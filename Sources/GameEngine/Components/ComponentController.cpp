#include "ComponentController.h"

#include <algorithm>
#include <magic_enum/magic_enum.hpp>

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
    for (auto& [gameObjectId, functionMap] : functions_)
    {
        if (functionMap.size() > 0)
        {
            LOG_WARN << "Some funstion left. GameObjectId : " << gameObjectId;
            for (const auto& [type, _] : functionMap)
            {
                LOG_WARN << " GameObjectId : " << gameObjectId << "Function type " << magic_enum::enum_name(type);
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
        LOG_WARN << "ComponentsType not found.";
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
        LOG_WARN << "Function not found.";
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
                LOG_ERROR << "Function id=" << id << "of type{"
                          << magic_enum::enum_name(type) << "} not found for gameObjectId=" << gameObjectId;
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
