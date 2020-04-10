#pragma once
#include <functional>
#include <unordered_map>
#include "ComponentsTypes.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
class IComponent;

enum class FunctionType
{
    Awake,
    OnStart,
    Update,
    PostUpdate
};

typedef std::unordered_map<uint32, std::function<void()> > FunctionMap;
typedef std::unordered_map<uint32, IComponent*> RegistredComponentsMap;

class ComponentController final
{
public:
    ComponentController();
    ~ComponentController();

    const RegistredComponentsMap &GetAllComonentsOfType(ComponentsType) const;

    uint32 RegisterFunction(FunctionType, std::function<void()>);
    void UnRegisterFunction(FunctionType, uint32);

    uint32 RegisterComponent(ComponentsType, IComponent*);
    void UnRegisterComponent(ComponentsType, uint32);

    void UnRegisterAll();

public:
    void OnStart();
    void Update();
    void PostUpdate();
    void CallFunc(FunctionType type);

private:
    void CallFunctionIfControllerStarted(FunctionType type, std::function<void()> func);

private:
    std::unordered_map<FunctionType, FunctionMap> functions_;
    std::unordered_map<ComponentsType, RegistredComponentsMap> registredComponents_;
    uint32 functionId;
    uint32 componentId;
    bool isStarted;
};
}  // namespace Components
}  // namespace GameEngine
