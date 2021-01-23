#pragma once
#include <Utils/IdPool.h>
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
    PostUpdate,
    AlwaysUpdate,
};

typedef std::unordered_map<uint32, IComponent *> RegistredComponentsMap;

class ComponentController final
{
public:
    struct ComponentFunction
    {
        IdType id{0};
        bool isActive{true};
        std::function<void()> function;
    };

    using GameObjectId = IdType;
    using ComponentFunctions =
        std::unordered_map<GameObjectId, std::unordered_map<FunctionType, std::vector<ComponentFunction>>>;

    ComponentController();
    ~ComponentController();

    const RegistredComponentsMap &GetAllComonentsOfType(ComponentsType) const;

    uint32 RegisterFunction(IdType, FunctionType, std::function<void()>);
    void UnRegisterFunction(GameObjectId, FunctionType, uint32);
    void setActivateStateOfComponentFunction(GameObjectId, FunctionType, uint32, bool);

    uint32 RegisterComponent(ComponentsType, IComponent *);
    void UnRegisterComponent(ComponentsType, uint32);

    void UnRegisterAll();

public:
    void OnObjectCreated(IdType);
    void OnStart();
    void Update();
    void PostUpdate();
    void AlwaysUpdate();
    void CallFunctions(FunctionType);
    void CallGameObjectFunctions(FunctionType, IdType);

private:
    ComponentFunctions functions_;
    std::unordered_map<ComponentsType, RegistredComponentsMap> registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;
};
}  // namespace Components
}  // namespace GameEngine
