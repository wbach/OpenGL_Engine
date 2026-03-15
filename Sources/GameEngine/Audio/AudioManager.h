#pragma once
#include <EngineApi.h>

#include <memory>

#include "IAudioManager.h"

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
};
}  // namespace GameEngine
