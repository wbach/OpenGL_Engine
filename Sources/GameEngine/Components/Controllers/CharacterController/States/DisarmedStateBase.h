#pragma once
#include "../AnimationClipNames.h"

namespace GameEngine
{
namespace Components
{
class DisarmedStateBase
{
public:
    DisarmedStateBase(const AnimationClipsNames& c)
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
