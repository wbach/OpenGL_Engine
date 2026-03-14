#include "AudioManager.h"

#include <Logger/Log.h>

#include <memory>

#include "GameEngine/Resources/File.h"
#include "Utils.h"

#define MINIAUDIO_IMPLEMENTATION
#ifndef MA_NO_VORBIS
#include "../Tools/common/miniaudio-0.11.25/extras/stb_vorbis.c"
#endif
#include <miniaudio.h>

namespace GameEngine
{
struct AudioManager::Pimpl
{
    Pimpl()
    {
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS)
        {
            LOG_DEBUG << "Audio init failure";
            return;
        }
        initStatus = true;
    }

    void play(const File& file)
    {
        if (not initStatus)
            return;

        LOG_DEBUG << "Play sound : " << file << ", file exist?  "
                  << Utils::BoolToString(std::filesystem::exists(file.GetAbsolutePath()));
        ma_engine_play_sound(&engine, file.GetAbsolutePath().string().c_str(), NULL);
    }

    ~Pimpl()
    {
        ma_engine_uninit(&engine);
    }
    ma_result result;
    ma_engine engine;

    bool initStatus{false};
};

AudioManager::AudioManager()
    : impl{std::make_unique<Pimpl>()}
{
}
AudioManager::~AudioManager()
{
}
void AudioManager::play(const File& file)
{
    if (not impl)
        return;

    impl->play(file);
}
}  // namespace GameEngine
