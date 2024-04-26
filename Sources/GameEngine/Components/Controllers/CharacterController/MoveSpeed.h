#pragma once
#include <Utils/math.hpp>

namespace GameEngine
{
namespace Components
{
const float DEFAULT_CROUCH_SPEED          = Utils::KmToMs(3.f);
const float DEFAULT_WALK_SPEED            = Utils::KmToMs(6.f);
const float DEFAULT_RUN_SPEED             = Utils::KmToMs(12.f);
const float DEFAULT_SPRINT_SPEED          = Utils::KmToMs(18.f);
const float DEFAULT_RUN_LEFT_RIGHT_SPEED  = DEFAULT_RUN_SPEED * 0.8f;
const float DEFAULT_WALK_LEFT_RIGHT_SPEED = DEFAULT_WALK_SPEED * 0.8f;
const float DEFAULT_BACKWARD_RUN_SPEED    = DEFAULT_RUN_SPEED * 0.75f;
const float DEFAULT_BACKWARD_WALK_SPEED   = DEFAULT_WALK_SPEED * 0.75f;
const float DEFAULT_MOVING_CHANGE_DIR_SPEED = 0.5f;

const float DEFAULT_TURN_SPEED        = 160.f;
const float DEFAULT_CROUCH_TURN_SPEED = 100.f;
const float DEFAULT_JUMP_POWER        = 5.f;

struct MoveSpeed
{
    float forward{0};
    float backward{0};
    float leftRight{0};
    float rotate{0};
};
}  // namespace Components
}  // namespace GameEngine
