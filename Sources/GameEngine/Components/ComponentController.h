#pragma once
#include <functional>
#include <unordered_map>
#include "ComponentsTypes.h"
#include "Types.h"
#include <Utils/IdPool.h>

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
    using GameObjectId = IdType;

    struct ComponentFunction
    {
        IdType id;
        std::function<void()> function;
    };

    ComponentController();
    ~ComponentController();

    const RegistredComponentsMap &GetAllComonentsOfType(ComponentsType) const;

    uint32 RegisterFunction(IdType, FunctionType, std::function<void()>);
    void UnRegisterFunction(GameObjectId, FunctionType, uint32);

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
    std::unordered_map<GameObjectId, std::unordered_map<FunctionType, std::vector<ComponentFunction>>> functions_;
    std::unordered_map<ComponentsType, RegistredComponentsMap> registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;
};
}  // namespace Components
}  // namespace GameEngine
