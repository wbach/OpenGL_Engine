#pragma once
#include <functional>
#include <unordered_map>
#include "Types.h"

namespace GameEngine
{
namespace Components
{
enum class FunctionType
{
    Awake,
    OnStart,
    Update,
    PostUpdate
};

typedef std::unordered_map<uint32, std::function<void()> > FunctionMap;

class ComponentController
{
public:
    ComponentController();
    virtual ~ComponentController();
    uint32 RegisterFunction(FunctionType type, std::function<void()> func);
    void UnRegisterFunction(FunctionType type, uint32 id);

public:
    void OnAwake();
    void OnStart();
    void Update();
    void PostUpdate();
    void CallFunc(FunctionType type);

private:
    void CallFunctionIfControllerStarted(FunctionType type, std::function<void()> func);

private:
    std::unordered_map<FunctionType, FunctionMap> functions_;
    uint32 functionId;
    bool isStarted;
    bool isAwake;
};
}  // namespace Components
}  // namespace GameEngine
