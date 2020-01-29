#pragma once

namespace common
{
enum TransformMessageTypes
{
    NONE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    ROTATE_RIGHT,
    ROTATE_LEFT,
    JUMP
};
enum TransformAction
{
    PUSH,
    POP
};
}  // namespace Network
