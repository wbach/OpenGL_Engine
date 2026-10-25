#pragma once
#include "../AnimationClipNames.h"

namespace GameEngine
{
namespace Components
{
class Armed
{
public:
    Armed(const AnimationClipsNames& c)
        : clips(c.armed)
    {
    }

    const StateClipsNames& getClips() const
    {
        return clips;
    }

protected:
    StateClipsNames clips;
};
}  // namespace Components
}  // namespace GameEngine
