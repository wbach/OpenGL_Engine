#pragma once
#include <functional>
#include "Types.h"

namespace GameEngine
{
enum SceneEventType
{
    RELOAD_SCENE,
    LOAD_NEXT_SCENE,
    LOAD_PREVIOUS_SCENE,
    LOAD_SCENE_BY_NAME,
    LOAD_SCENE_BY_ID,
    LOAD_FROM_FILE
};

struct SceneEvent
{
    SceneEvent(SceneEventType t, uint32 id)
        : SceneEvent(t, "", id)
    {
    }
    SceneEvent(SceneEventType t = SceneEventType::RELOAD_SCENE, const std::string& n = "", uint32 i = 0)
        : type(t)
        , name(n)
        , id(i)
    {
    }

    SceneEventType type;
    std::string name;
    uint32 id;
};

typedef std::function<void(const SceneEvent&)> AddEvent;
}  // namespace GameEngine
