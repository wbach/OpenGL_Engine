#include "MoveController.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
void MoveController::moveLeft()
{
    leftDir = 1.f;
    updateDir();
}

void MoveController::moveRight()
{
    rightDir = 1.f;
    updateDir();
}

void MoveController::moveForwad()
{
    forwardDir = 1.f;
    updateDir();
}

void MoveController::moveBackward()
{
    backwardDir = 1.f;
    updateDir();
}

void MoveController::endMoveLeft()
{
    leftDir = 0.f;
    updateDir();
}

void MoveController::endMoveRight()
{
    rightDir = 0.f;
    updateDir();
}

void MoveController::endMoveForwad()
{
    forwardDir = 0.f;
    updateDir();
}

void MoveController::endMoveBackward()
{
    backwardDir = 0.f;
    updateDir();
}

bool MoveController::isMoving() const
{
    return glm::length(moveDirection) > std::numeric_limits<float>::epsilon();
}

bool MoveController::isMoving(Direction dir) const
{
    switch (dir)
    {
        case MoveController::Direction::Left:
            return moveDirection.x > 0.5f;
        case MoveController::Direction::Right:
            return moveDirection.x < -0.5f;
        case MoveController::Direction::Forward:
            return moveDirection.z > 0.5f;
        case MoveController::Direction::Backward:
            return moveDirection.z < -0.5f;
    }
    return false;
}

bool MoveController::isMoveActive() const
{
    // /*DISABLED*/ DEBUG_LOG(std::to_string(leftDir));
    // /*DISABLED*/ DEBUG_LOG(std::to_string(rightDir));
    // /*DISABLED*/ DEBUG_LOG(std::to_string(forwardDir));
    // /*DISABLED*/ DEBUG_LOG(std::to_string(backwardDir));
    return leftDir > 0.5f or rightDir > 0.5f or forwardDir > 0.5f or backwardDir > 0.5f;
}

const vec3 &MoveController::getCurrentDir() const
{
    return moveDirection;
}

void MoveController::updateDir()
{
    moveDirection = vec3(leftDir - rightDir, 0, forwardDir - backwardDir);
}

}  // namespace Components
}  // namespace GameEngine
