#pragma once
#include <GameEngine/Audio/IAudioManager.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class AudioManagerMock : public IAudioManager
{
public:
    MOCK_METHOD(void, play, (const File&), (override));
};
}  // namespace GameEngine
