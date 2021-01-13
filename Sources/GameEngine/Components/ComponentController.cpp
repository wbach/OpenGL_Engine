#include "ComponentController.h"

#include <algorithm>

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const RegistredComponentsMap DEFAULT_COMPONETNS_MAP;
}

ComponentController::ComponentController()
    : componentId(0)
    , isStarted(false)
{
}
ComponentController::~ComponentController()
{
    DEBUG_LOG("destructor");

    for (auto& f : functions_)
    {
        if (f.second.size() > 0)
        {
            WARNING_LOG("Some funstion left.");
        }
    }

    for (auto& f : registredComponents_)
    {
        if (f.second.size() > 0)
        {
            WARNING_LOG("Some components left.");
        }
    }
}

const RegistredComponentsMap& ComponentController::GetAllComonentsOfType(ComponentsType type) const
{
    auto iter = registredComponents_.find(type);

    if (iter != registredComponents_.end())
    {
        return iter->second;
    }

    return DEFAULT_COMPONETNS_MAP;
}
uint32 ComponentController::RegisterFunction(IdType gameObjectId, FunctionType type, std::function<void()> func)
{
    auto id = functionIdsPool_.getId();
    functions_[gameObjectId][type].push_back({id, func});
    return id;
}

uint32 ComponentController::RegisterComponent(ComponentsType type, IComponent* component)
{
    auto currentComponentId = componentId++;

    registredComponents_[type].insert({currentComponentId, component});
    return currentComponentId;
}

void ComponentController::UnRegisterComponent(ComponentsType type, uint32 id)
{
    if (registredComponents_.count(type) > 0)
    {
        registredComponents_.at(type).erase(id);
    }
    else
    {
        WARNING_LOG("ComponentsType not found.");
    }
}
void ComponentController::UnRegisterFunction(ComponentController::GameObjectId gameObjectId, FunctionType type,
                                             uint32 id)
{
    auto iter = functions_.find(gameObjectId);
    if (iter != functions_.end())
    {
        auto typeIter = iter->second.find(type);

        if (typeIter != iter->second.end())
        {
            auto functionIter =
                std::find_if(typeIter->second.begin(), typeIter->second.end(),
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
    }
}
void ComponentController::OnStart()
{
    CallFunctions(FunctionType::OnStart);
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
