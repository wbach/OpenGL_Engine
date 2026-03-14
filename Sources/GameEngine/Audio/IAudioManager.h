#pragma once
#include <EngineApi.h>

namespace GameEngine
{
class File;

class ENGINE_API IAudioManager
{
public:
    virtual ~IAudioManager() = default;
    virtual void play(const File&) = 0;
};
}  // namespace GameEngine
