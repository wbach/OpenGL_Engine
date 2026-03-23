#pragma once
#include <EngineApi.h>
#include <Types.h>

#include <magic_enum/magic_enum.hpp>
#include <optional>

#include "Attenuation.h"
#include "AudioId.h"
#include "PlayType.h"
#include "SoundCone.h"

namespace GameEngine
{
using Position  = vec3;
using Direction = vec3;
using Volume    = float;

struct ENGINE_API PlayParameters
{
    std::optional<Position> position;
    std::optional<Direction> direction;
    std::optional<SoundCone> cone;
    std::optional<Volume> volume{1.0f};

    std::function<void()> playEndCallback{nullptr};
    PlayType playType{PlayType::Once};
    Attenuation attenuation;

    std::optional<float> minDistance;
    std::optional<float> maxDistance;
    std::optional<float> pitch{1.0f};
};

inline std::ostream& operator<<(std::ostream& os, const PlayParameters& params)
{
    os << "PlayParameters { ";
    os << "Type: " << magic_enum::enum_name(params.playType);

    if (params.volume)
        os << ", Vol: " << *params.volume;
    if (params.pitch)
        os << ", Pitch: " << *params.pitch;

    if (params.position)
    {
        os << ", Pos: [" << params.position->x << ", " << params.position->y << ", " << params.position->z << "]";
        os << "Attenuation: " << magic_enum::enum_name(params.attenuation);
    }
    if (params.direction)
        os << ", Dir: [" << params.direction->x << ", " << params.direction->y << ", " << params.direction->z << "]";

    if (params.minDistance)
        os << ", MinDist: " << *params.minDistance;
    if (params.maxDistance)
        os << ", MaxDist: " << *params.maxDistance;

    if (params.cone)
    {
        os << ", Cone: [In: " << params.cone->innerAngle << ", Out: " << params.cone->outerAngle
           << ", Gain: " << params.cone->outerGain << "]";
    }

    if (params.playEndCallback)
        os << ", HasCallback: Yes";
    else
        os << ", HasCallback: No";

    os << " }";
    return os;
}
}  // namespace GameEngine
