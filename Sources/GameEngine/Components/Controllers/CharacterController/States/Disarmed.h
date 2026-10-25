#pragma once
#include "../AnimationClipNames.h"

namespace GameEngine
{
namespace Components
{
class Disarmed
{
public:
    Disarmed(const AnimationClipsNames& c)
        : clips(c.disarmed)
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
