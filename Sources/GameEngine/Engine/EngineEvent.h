#pragma once
#include <variant>

#include "Types.h"
namespace GameEngine
{
enum class QuitEvent
{
    ASK_QUIT,
    QUIT
};

struct ChangeSceneEvent
{
    enum Type
    {
        RELOAD_SCENE,
        LOAD_NEXT_SCENE,
        LOAD_PREVIOUS_SCENE,
        LOAD_SCENE_BY_NAME,
        LOAD_SCENE_BY_ID,
        LOAD_FROM_FILE
    };

    ChangeSceneEvent(Type t, uint32 id)
        : ChangeSceneEvent(t, "", id)
    {
    }
    ChangeSceneEvent(Type t = Type::RELOAD_SCENE, const std::string& n = "", uint32 i = 0)
        : type(t)
        , name(n)
        , id(i)
    {
    }

    Type type;
    std::string name;
    uint32 id;
};

struct ChangeSceneConfirmEvent
{
    ChangeSceneEvent event;
};

using EngineEvent = std::variant<QuitEvent, ChangeSceneEvent, ChangeSceneConfirmEvent>;
}  // namespace GameEngine
