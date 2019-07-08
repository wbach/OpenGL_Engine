#include "ComponentController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
ComponentController::ComponentController()
    : functionId(0)
    , isStarted(false)
    , isAwake(false)
{
}
ComponentController::~ComponentController()
{
    DEBUG_LOG("");
}
uint32 ComponentController::RegisterFunction(FunctionType type, std::function<void()> func)
{
    CallFunctionIfControllerStarted(type, func);

    auto currentId              = functionId++;
    functions_[type][currentId] = func;
    return currentId;
}
void ComponentController::UnRegisterFunction(FunctionType type, uint32 id)
{
    if (functions_.count(type) > 0)
        functions_.at(type).erase(id);
}
void ComponentController::UnRegisterAll()
{
    functions_.clear();
}
void ComponentController::OnAwake()
{
    CallFunc(FunctionType::Awake);
    isAwake = true;
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
    for (const auto& funcPair : functions_[type])
        funcPair.second();
}
void ComponentController::CallFunctionIfControllerStarted(FunctionType type, std::function<void()> func)
{
    if (isStarted && type == FunctionType::OnStart)
        func();
    else if (isAwake && type == FunctionType::Awake)
        func();
}
}  // namespace Components
}  // namespace GameEngine
