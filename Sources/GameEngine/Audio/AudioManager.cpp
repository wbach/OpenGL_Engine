#include "AudioManager.h"

#include <Logger/Log.h>
#include <miniaudio.h>

#include <limits>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <string>

#include "GameEngine/Audio/AudioId.h"
#include "GameEngine/Audio/PlayParameters.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/File.h"
#include "IdPool.h"
#include "Types.h"
#include "Utils.h"

namespace GameEngine
{
namespace
{
struct SoundEndData
{
    std::function<void()> callback;
};

static void on_ma_sound_end(void* pUserData, ma_sound* pSound)
{
    auto* data = static_cast<SoundEndData*>(pUserData);
    if (data->callback)
    {
        data->callback();
    }
    delete data;
}
}  // namespace
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

        for (auto group : magic_enum::enum_values<PlayGroup>())
        {
            if (group == PlayGroup::Master)
                continue;

            ma_result groupRes = ma_sound_group_init(&engine, 0, nullptr, &groups[group]);
            if (groupRes != MA_SUCCESS)
            {
                LOG_ERROR << "Failed to init audio group: " << magic_enum::enum_name(group);
            }
        }

        initStatus = true;
    }

    AudioId play(const File& file, PlayGroup group, const PlayParameters& params)
    {
        if (not initStatus)
            return INVALID_AUDIO_ID;

        LOG_DEBUG << "Play sound : " << file << ", file exist?  "
                  << Utils::BoolToString(std::filesystem::exists(file.GetAbsolutePath()));
        // ma_engine_play_sound(&engine, file.GetAbsolutePath().string().c_str(), NULL);

        auto sound             = std::make_unique<ma_sound>();
        ma_sound_group* pGroup = (group == PlayGroup::Master) ? nullptr : &groups[group];

        /*
         * FLAGI (ma_uint32):
         * - MA_SOUND_FLAG_DECODE: Rozpakowuje cały plik do RAM. Idealne dla krótkich SFX (brak lagów przy odczycie).
         * - MA_SOUND_FLAG_STREAM: Czyta plik w locie z dysku. Obowiązkowe dla długiej muzyki (oszczędność RAM).
         * - MA_SOUND_FLAG_NO_SPATIALIZATION: Wyłącza silnik 3D (panoramę, tłumienie). Używamy dla UI/2D.
         */

        ma_result result = ma_sound_init_from_file(&engine, file.GetAbsolutePath().string().c_str(),
                                                   getFlagsForGroup(group, params), pGroup, nullptr, sound.get());

        if (result != MA_SUCCESS)
        {
            LOG_ERROR << "Failed to load sound: " << file;
            return INVALID_AUDIO_ID;
        }

        if (params.volume.has_value())
        {
            ma_sound_set_volume(sound.get(), params.volume.value());
        }

        if (params.playType == PlayType::Loop)
        {
            ma_sound_set_looping(sound.get(), MA_TRUE);
        }

        if (params.direction.has_value())
        {
            ma_sound_set_direction(sound.get(), params.direction->x, params.direction->y, params.direction->z);
        }

        if (params.pitch)
        {
            ma_sound_set_pitch(sound.get(), *params.pitch);
        }

        if (params.cone.has_value())
        {
            ma_sound_set_cone(sound.get(), params.cone->innerAngle, params.cone->outerAngle, params.cone->outerGain);
        }

        if (params.position.has_value())
        {
            ma_sound_set_position(sound.get(), params.position->x, params.position->y, params.position->z);
        }

        if (params.playEndCallback)
        {
            auto* data = new SoundEndData{params.playEndCallback};
            ma_sound_set_end_callback(sound.get(), on_ma_sound_end, data);
        }

        if (params.minDistance)
        {
            ma_sound_set_min_distance(sound.get(), *params.minDistance);
        }

        if (params.maxDistance)
        {
            ma_sound_set_min_distance(sound.get(), *params.maxDistance);
        }

        ma_sound_start(sound.get());

        AudioId id{audioIdPool.getId()};
        activeSounds[id] = std::move(sound);

        return id;
    }

    ma_uint32 getFlagsForGroup(PlayGroup group, const PlayParameters& params)
    {
        ma_uint32 flags = 0;

        switch (group)
        {
            case PlayGroup::Music:
            case PlayGroup::Dialogs:
                flags |= MA_SOUND_FLAG_STREAM;
                break;

            case PlayGroup::SFX:
            case PlayGroup::Barks:
            default:
                flags |= MA_SOUND_FLAG_DECODE;
                break;
        }

        bool isAlways2D    = (group == PlayGroup::Music or group == PlayGroup::Dialogs);
        bool hasNoPosition = !params.position.has_value();

        if (isAlways2D or hasNoPosition)
        {
            flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;
        }

        return flags;
    }

    ~Pimpl()
    {
        ma_engine_uninit(&engine);
    }
    ma_result result;
    ma_engine engine;

    bool initStatus{false};

    Utils::IdPool audioIdPool;

    std::unordered_map<PlayGroup, ma_sound_group> groups;
    std::unordered_map<AudioId, std::unique_ptr<ma_sound>> activeSounds;
};

AudioManager::AudioManager()
    : impl{std::make_unique<Pimpl>()}
{
    audioEnabled.store(EngineConf.sound.isEnabled);
    audioEnabledSubId = EngineConf.sound.isEnabled.subscribeForChange(
        [this]()
        {
            audioEnabled.store(EngineConf.sound.isEnabled);
            if (not EngineConf.sound.isEnabled)
            {
                stopAll();
            }
        });

    audioEnabledSubId = EngineConf.sound.volume.subscribeForChange([this]() { setMasterVolume(EngineConf.sound.volume); });
}
AudioManager::~AudioManager()
{
    EngineConf.sound.isEnabled.unsubscribe(audioEnabledSubId);
    EngineConf.sound.volume.unsubscribe(volumneSubId);
}
AudioId AudioManager::play(const File& file, PlayGroup group, const PlayParameters& params)
{
    if (not impl or not audioEnabled.load() or not file.exist())
        return INVALID_AUDIO_ID;

    return impl->play(file, group, params);
}

void AudioManager::stop(AudioId id)
{
    if (not impl)
        return;

    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        ma_sound_set_end_callback(it->second.get(), nullptr, nullptr);
        ma_sound_stop(it->second.get());
        ma_sound_uninit(it->second.get());
        impl->audioIdPool.releaseId(id.value);
        impl->activeSounds.erase(it);
    }
}
void AudioManager::pause(AudioId id)
{
    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        ma_sound_stop(it->second.get());
    }
}
void AudioManager::resume(AudioId id)
{
    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        ma_sound_start(it->second.get());
    }
}
bool AudioManager::isPlaying(AudioId id) const
{
    if (not impl)
        return false;

    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        return ma_sound_is_playing(it->second.get());
    }
    return false;
}
void AudioManager::setVolume(AudioId id, Volume volume)
{
    if (not impl)
        return;

    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
        ma_sound_set_volume(it->second.get(), volume);
}
void AudioManager::setPitch(AudioId id, float pitch)
{
    if (not impl)
        return;
    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
        ma_sound_set_pitch(it->second.get(), pitch);
}
void AudioManager::setSourcePosition(AudioId id, const Position& pos)
{
    if (not impl)
        return;
    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
        ma_sound_set_position(it->second.get(), pos.x, pos.y, pos.z);
}
void AudioManager::setSourceDirection(AudioId id, const Direction& dir)
{
    if (not impl)
        return;

    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        ma_sound_set_direction(it->second.get(), dir.x, dir.y, dir.z);
    }
}
void AudioManager::setListenerTransform(const Position& pos, const Direction& forward, const Direction& up)
{
    if (not impl or not impl->initStatus)
        return;

    ma_engine_listener_set_position(&impl->engine, 0, pos.x, pos.y, pos.z);
    ma_engine_listener_set_direction(&impl->engine, 0, forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(&impl->engine, 0, up.x, up.y, up.z);
}
void AudioManager::setMasterVolume(Volume volume)
{
    if (not impl or not impl->initStatus)
        return;

    ma_engine_set_volume(&impl->engine, volume);
}
float AudioManager::getMasterVolume() const
{
    if (not impl or not impl->initStatus)
        return 1.0f;
    return ma_engine_get_volume(&impl->engine);
}
void AudioManager::setGroupVolume(PlayGroup group, Volume volume)
{
    if (not impl or not impl->initStatus)
        return;

    if (group == PlayGroup::Master)
    {
        ma_engine_set_volume(&impl->engine, volume);
        return;
    }

    auto it = impl->groups.find(group);
    if (it != impl->groups.end())
        ma_sound_group_set_volume(&it->second, volume);
}
void AudioManager::stopAll()
{
    if (!impl)
        return;

    for (auto it = impl->activeSounds.begin(); it != impl->activeSounds.end();)
    {
        ma_sound_set_end_callback(it->second.get(), nullptr, nullptr);
        ma_sound_stop(it->second.get());
        ma_sound_uninit(it->second.get());

        impl->audioIdPool.releaseId(it->first.value);

        it = impl->activeSounds.erase(it);
    }

    ma_engine_stop(&impl->engine);
    ma_engine_start(&impl->engine);

    LOG_DEBUG << "Audio stopped for all active sounds.";
}
void AudioManager::update()
{
    if (not impl)
        return;

    for (auto it = impl->activeSounds.begin(); it != impl->activeSounds.end();)
    {
        if (ma_sound_at_end(it->second.get()))
        {
            ma_sound_uninit(it->second.get());
            impl->audioIdPool.releaseId(it->first.value);
            it = impl->activeSounds.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
void AudioManager::finish(AudioId id)
{
    if (not impl)
        return;

    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        auto* sound = it->second.get();
        ma_sound_stop(sound);

        void* pUserData = sound->pEndCallbackUserData;

        if (pUserData)
        {
            auto* data = static_cast<SoundEndData*>(pUserData);

            ma_sound_set_end_callback(sound, nullptr, nullptr);

            if (data->callback)
            {
                data->callback();
            }

            delete data;
        }

        ma_sound_uninit(sound);
        impl->audioIdPool.releaseId(id.value);
        impl->activeSounds.erase(it);

        LOG_DEBUG << "Sound finished manually with callback: " << id;
    }
}
void AudioManager::setAttenuation(AudioId id, float minDistance, float maxDistance)
{
    if (not impl)
        return;
    auto it = impl->activeSounds.find(id);
    if (it != impl->activeSounds.end())
    {
        ma_sound_set_attenuation_model(it->second.get(), ma_attenuation_model_inverse);

        ma_sound_set_min_distance(it->second.get(), minDistance);
        ma_sound_set_max_distance(it->second.get(), maxDistance);
    }
}
}  // namespace GameEngine
