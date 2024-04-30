#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Components
{
class MoveController
{
public:
    enum class Direction
    {
        Left,
        Right,
        Forward,
        Backward
    };
    void moveLeft();
    void moveRight();
    void moveForwad();
    void moveBackward();

    void endMoveLeft();
    void endMoveRight();
    void endMoveForwad();
    void endMoveBackward();

    bool isMoving() const;
    bool isMoving(Direction) const;
    bool isMoveActive() const;
    const vec3& getCurrentDir() const;

private:
    void updateDir();

private:
    float leftDir{0.f};
    float rightDir{0.f};
    float forwardDir{0.f};
    float backwardDir{0.f};

    vec3 moveDirection{0.f};
};
}  // namespace Components
}  // namespace GameEngine
