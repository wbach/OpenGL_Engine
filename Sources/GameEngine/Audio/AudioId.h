#pragma once
#include <Types.h>

#include <compare>
#include <functional>

namespace GameEngine
{
struct AudioId
{
    IdType value;
    auto operator<=>(const AudioId&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const AudioId& id)
    {
        return os << "AudioId(" << id.value << ")";
    }
};

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
