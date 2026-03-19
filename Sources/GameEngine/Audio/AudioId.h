#pragma once
#include <EngineApi.h>
#include <Types.h>

#include <compare>
#include <functional>

namespace GameEngine
{
struct ENGINE_API AudioId
{
    IdType value{std::numeric_limits<IdType>::max()};
    auto operator<=>(const AudioId&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const AudioId& id)
    {
        return os << "AudioId(" << id.value << ")";
    }
};

static const AudioId INVALID_AUDIO_ID{};
}  // namespace GameEngine

namespace std
{
template <>
struct hash<GameEngine::AudioId>
{
    size_t operator()(const GameEngine::AudioId& id) const noexcept
    {
        return std::hash<IdType>{}(id.value);
    }
};

}  // namespace std
