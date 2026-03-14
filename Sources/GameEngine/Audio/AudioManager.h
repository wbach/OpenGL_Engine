#pragma once
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
    void play(const File&) override;

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl;
};
}  // namespace GameEngine
