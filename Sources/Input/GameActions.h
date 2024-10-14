#pragma once

namespace Input
{
enum class GameAction
{
    MOVE_FORWARD = 0,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    EQUIP_DISARM,
    CROUCH,
    WALK,
    SPRINT,
    JUMP,
    DODGE_DIVE,
    DODGE,
    AIM,
    ATTACK,
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT,
    CAM_TURN_LEFT,
    CAM_TURN_RIGHT,
    CAM_PITCH_UP,
    CAM_PITCH_DOWN,
};
}
