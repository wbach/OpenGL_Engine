#pragma once
#include <EngineApi.h>

#include <atomic>
#include <memory>

#include "IAudioManager.h"
#include "Types.h"

namespace GameEngine
{
class File;

class ENGINE_API AudioManager : public IAudioManager
{
public:
    AudioManager();
    ~AudioManager() override;

    AudioId play(const File&, PlayGroup, const PlayParameters& = {}) override;

    void stop(AudioId) override;
    void pause(AudioId) override;
    void resume(AudioId) override;
    void finish(AudioId) override;
    void update() override;

    bool isPlaying(AudioId) const override;

    void setVolume(AudioId, Volume) override;
    void setPitch(AudioId, float) override;
    void setSourcePosition(AudioId, const Position&) override;
    void setSourceDirection(AudioId, const Direction&) override;

    void setListenerTransform(const Position&, const Direction&, const Direction&) override;

    void setMasterVolume(Volume) override;
    float getMasterVolume() const override;

    void setGroupVolume(PlayGroup, Volume) override;
    void stopAll() override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl;

    IdType audioEnabledSubId{0};
    IdType volumneSubId{0};
    std::atomic_bool audioEnabled{true};
};
}  // namespace GameEngine
