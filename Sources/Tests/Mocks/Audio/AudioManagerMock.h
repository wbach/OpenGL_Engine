#pragma once
#include <GameEngine/Audio/IAudioManager.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class AudioManagerMock : public IAudioManager
{
public:
    virtual ~AudioManagerMock() = default;

    MOCK_METHOD(AudioId, play, (const File&, PlayGroup, const PlayParameters&), (override));
    MOCK_METHOD(void, update, (), (override));
    MOCK_METHOD(void, stop, (AudioId), (override));
    MOCK_METHOD(void, pause, (AudioId), (override));
    MOCK_METHOD(void, resume, (AudioId), (override));
    MOCK_METHOD(void, finish, (AudioId), (override));
    MOCK_METHOD(bool, isPlaying, (AudioId), (const, override));
    MOCK_METHOD(void, setAttenuation, (AudioId, float, float), (override));

    MOCK_METHOD(void, setVolume, (AudioId, Volume), (override));
    MOCK_METHOD(void, setPitch, (AudioId, float), (override));
    MOCK_METHOD(void, setSourcePosition, (AudioId, const Position&), (override));
    MOCK_METHOD(void, setSourceDirection, (AudioId, const Direction&), (override));

    MOCK_METHOD(void, setListenerTransform, (const Position&, const Direction&, const Direction&), (override));

    MOCK_METHOD(void, setMasterVolume, (Volume), (override));
    MOCK_METHOD(float, getMasterVolume, (), (const, override));
    MOCK_METHOD(void, setGroupVolume, (PlayGroup, Volume), (override));
    MOCK_METHOD(void, stopAll, (), (override));
};
}  // namespace GameEngine
