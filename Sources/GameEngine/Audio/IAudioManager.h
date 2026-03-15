#pragma once
#include <EngineApi.h>
#include <Types.h>

#include <functional>
#include <glm/glm.hpp>
#include <optional>

#include "PlayGroup.h"
#include "PlayParameters.h"

namespace GameEngine
{
class File;

class ENGINE_API IAudioManager
{
public:
    virtual ~IAudioManager() = default;

    virtual AudioId play(const File&, PlayGroup = PlayGroup::SFX, const PlayParameters& = {}) = 0;

    virtual void update()                 = 0;
    virtual void stop(AudioId)            = 0;
    virtual void pause(AudioId)           = 0;
    virtual void resume(AudioId)          = 0;
    virtual bool isPlaying(AudioId) const = 0;

    virtual void setVolume(AudioId, Volume)                       = 0;
    virtual void setPitch(AudioId, float)                         = 0;
    virtual void setSourcePosition(AudioId id, const Position&)   = 0;
    virtual void setSourceDirection(AudioId id, const Direction&) = 0;

    virtual void setListenerTransform(const Position&, const Direction&, const Direction&) = 0;

    virtual void setMasterVolume(Volume)  = 0;
    virtual float getMasterVolume() const = 0;

    virtual void setGroupVolume(PlayGroup, Volume) = 0;
    virtual void stopAll()                         = 0;
};
}  // namespace GameEngine