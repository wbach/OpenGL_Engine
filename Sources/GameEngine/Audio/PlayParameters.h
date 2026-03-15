#pragma once
#include <EngineApi.h>
#include <Types.h>
#include <optional>

#include "PlayType.h"
#include "SoundCone.h"

namespace GameEngine
{
using Position  = glm::vec3;
using Direction = glm::vec3;
using Volume    = float;
using AudioId   = IdType;

struct ENGINE_API PlayParameters
{
    std::optional<Position> position;
    std::optional<Direction> direction;
    std::optional<SoundCone> cone;
    std::optional<Volume> volume{1.0f};

    std::function<void()> playEndCallback{nullptr};
    PlayType playType{PlayType::Once};

    std::optional<float> pitch{1.0f};
};
}  // namespace GameEngine
