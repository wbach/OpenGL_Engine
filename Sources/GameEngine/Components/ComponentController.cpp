#include "ComponentController.h"

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
    : functionId(0)
    , componentId(0)
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
    if (registredComponents_.count(type))
        return registredComponents_.at(type);

    return DEFAULT_COMPONETNS_MAP;
}
uint32 ComponentController::RegisterFunction(FunctionType type, std::function<void()> func)
{
    CallFunctionIfControllerStarted(type, func);

    auto currentId              = functionId++;
    functions_[type][currentId] = func;
    return currentId;
}

uint32 ComponentController::RegisterComponent(ComponentsType type, IComponent* component)
{
    auto currentComponentId = componentId++;

    registredComponents_[type][currentComponentId] = component;
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
void ComponentController::UnRegisterFunction(FunctionType type, uint32 id)
{
    if (functions_.count(type) > 0)
    {
        functions_.at(type).erase(id);
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
void ComponentController::OnStart()
{
    CallFunc(FunctionType::OnStart);
    isStarted = true;
}
void ComponentController::Update()
{
    CallFunc(FunctionType::Update);
}
void ComponentController::PostUpdate()
{
    CallFunc(FunctionType::PostUpdate);
}
void ComponentController::CallFunc(FunctionType type)
{
    if (functions_.count(type) > 0)
    {
        for (const auto& funcPair : functions_.at(type))
            funcPair.second();
    }
}
void ComponentController::CallFunctionIfControllerStarted(FunctionType type, std::function<void()> func)
{
    if (type == FunctionType::Awake)
        func();
    else if (isStarted && type == FunctionType::OnStart)
        func();
}
}  // namespace Components
}  // namespace GameEngine
