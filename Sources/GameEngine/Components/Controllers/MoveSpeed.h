#pragma once
#include <Utils/math.hpp>

namespace GameEngine
{
namespace Components
{

const float DEFAULT_RUN_SPEED  = Utils::KmToMs(12.f);
const float DEFAULT_TURN_SPEED = 160.f;
const float DEFAULT_JUMP_POWER = 10.f;
const float DEFAULT_BACKWARD_RUN_SPEED = DEFAULT_RUN_SPEED * 0.75f;

struct MoveSpeed
{
    float forward{DEFAULT_RUN_SPEED};
    float backward{DEFAULT_BACKWARD_RUN_SPEED};
    float leftRight{DEFAULT_TURN_SPEED};
    float rotate{DEFAULT_JUMP_POWER};
};
}  // namespace Components
}  // namespace GameEngine
