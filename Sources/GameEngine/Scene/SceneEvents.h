#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <variant>

#include "Rotation.h"
#include "Types.h"

namespace GameEngine
{
class GameObject;
struct AddGameObjectEvent
{
    IdType parentGameObject{0};
    std::unique_ptr<GameObject> gameObject;
};

struct AddGameObjectNotifEvent
{
    IdType parentGameObject{0};
    GameObject* gameObject{nullptr};
};

struct RemoveGameObjectEvent
{
    IdType gameObjectId{0};
};

struct ClearGameObjectsEvent
{
};

struct ChangeParentEvent
{
    IdType newParentId{0};
    IdType gameObjectId{0};
};

struct ModifyGameObjectEvent
{
    struct SetTransform
    {
        std::optional<vec3> position;
        std::optional<Rotation> rotation;
        std::optional<vec3> scale;
        std::optional<mat4> matrix;
    };

    IdType gameObjectId{0};
    std::optional<SetTransform> localTransform;
    std::optional<SetTransform> worldTransform;
};

// ChangeSceneEvent, ChangeSceneConfirmEvent,
using SceneEvent =
    std::variant<AddGameObjectEvent, ModifyGameObjectEvent, RemoveGameObjectEvent, ChangeParentEvent, ClearGameObjectsEvent>;
using SceneNotifEvent =
    std::variant<AddGameObjectNotifEvent, ModifyGameObjectEvent, RemoveGameObjectEvent, ChangeParentEvent, ClearGameObjectsEvent>;
using AddSceneEvent = std::function<void(SceneEvent&&)>;
}  // namespace GameEngine
