#pragma once
#include <Utils/IdPool.h>

#include <functional>
#include <unordered_map>

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

typedef std::unordered_map<uint32, IComponent*> RegistredComponentsMap;

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

    template <class T>
    const RegistredComponentsMap& GetAllComonentsOfType() const
    {
        auto iter = registredComponents_.find(typeid(T).hash_code());

        if (iter != registredComponents_.end())
        {
            return iter->second;
        }

        return DEFAULT_COMPONETNS_MAP;
    }

    uint32 RegisterFunction(IdType, FunctionType, std::function<void()>);
    void UnRegisterFunction(GameObjectId, FunctionType, uint32);
    void setActivateStateOfComponentFunction(GameObjectId, FunctionType, uint32, bool);

    uint32 RegisterComponent(size_t, IComponent*);
    void UnRegisterComponent(size_t, uint32);

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
    std::unordered_map<size_t, RegistredComponentsMap> registredComponents_;
    Utils::IdPool functionIdsPool_;
    uint32 componentId;
    bool isStarted;

private:
    const RegistredComponentsMap DEFAULT_COMPONETNS_MAP;
};
}  // namespace Components
}  // namespace GameEngine
