#pragma once
#include <EngineApi.h>
#include <Types.h>

#include <optional>

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

    std::optional<float> minDistance;
    std::optional<float> maxDistance;
    std::optional<float> pitch{1.0f};
};
}  // namespace GameEngine
