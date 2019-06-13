#pragma once
#include <functional>
#include "Types.h"

namespace GameEngine
{
enum SceneEventType
{
    NONE,
    LOAD_NEXT_SCENE,
    LOAD_PREVIOUS_SCENE,
    LOAD_SCENE_BY_NAME,
    LOAD_SCENE_BY_ID,
    EXIT_GAME
};

struct SceneEvent
{
    SceneEvent(SceneEventType t, uint32 id)
        : SceneEvent(t, "", id)
    {
    }
    SceneEvent(SceneEventType t = SceneEventType::NONE, const std::string& n = "", uint32 i = 0)
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
